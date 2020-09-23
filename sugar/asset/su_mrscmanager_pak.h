/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2010 AtOnce Technologies
**
** This file is part of the Sugar core-system framework.
**
** This file and the associated product may be used and distributed
** under the terms of a current License as defined by AtOnce Technologies
** and appearing in the file LICENSE.TXT included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.atonce-technologies.com or email info@atonce-technologies.com
** for informations about Sugar Framework and AtOnce Technologies others products
** License Agreements.
**
** Contact info@atonce-technologies.com if any conditions of this license
** are not clear to you.
**
*****************************************************************LIC-HDR*/




struct PakDesc;
struct PakNode;
struct PakFile;
struct PakBranch;




struct PakDesc
{
	PakNode*	node;
	uint32		rid;
	uint32		tid;
	uint32		dataso;
	uint32		dataeo;
	uint32		contentbs;
};



struct PakNode
{
	PakNode*					prev;			// to lower-revision nodes
	PakNode*					next;			// to upper-revision nodes
	uint						refcnt;

								PakNode			( );
	virtual void				recycle			( Pool* ) = 0;

	virtual void				retain			( );
	virtual void				release			( );

	virtual pcstr				getLabel		( ) = 0;
	virtual uint32				getRevision		( ) = 0;
	virtual bool				getRsc			( uint32 inRID, PakDesc& outDesc ) = 0;
	virtual	uint32				getLoRID		( ) = 0;
	virtual	uint32				getHiRID		( ) = 0;

	virtual bool				startTransfer	( TransferParams inParams, TransferParams& outAlignedParams ) = 0;
	virtual void				abortTransfer	( ) = 0;
	virtual TransferState		probeTransfer	( ) = 0;
};



struct PakFile : public PakNode
{
	PString						filename;
	IPakDesigner::PakHeader		hdr;
	IPakDesigner::PakDesc*		toc;
	uint						toclen;
	IFileBurst*					fburst;

	static PakFile*				create			( pcstr infilename, Pool* inp );
	void						recycle			( Pool* inp );

	pcstr						getLabel		( );
	uint32						getRevision		( );
	bool						getRsc			( uint32 inRID, PakDesc& outDesc );
	uint32						getLoRID		( );
	uint32						getHiRID		( );

	bool						startTransfer	( TransferParams inParams, TransferParams& outAlignedParams );
	void						abortTransfer	( );
	TransferState				probeTransfer	( );
};



struct PakBranch
{
	PakBranch*					next;
	PakBranch*					prev;
	uint32						labelcrc;
	PakNode*					first;			// from highest revision to lowest (r12, r8, r7, r4, ..., r1)

	static PakBranch*			create			( uint inlabelcrc, Pool* inp );
	void						recycle			( Pool* inp );

	virtual bool				findRsc			( uint32 inRID, PakDesc& outDesc );
	uint32						getLoRID		( );
	uint32						getHiRID		( );

	virtual bool				hasNode			( PakNode* innode );
	virtual void				insertNode		( PakNode* innode );
	virtual void				removeNode		( PakNode* innode );
};



struct PakCloud
{
	PakBranch*					first;

								PakCloud		( );
	void						clear			( Pool* inp );

	bool						findRsc			( uint32 inRID, PakDesc& outDesc );
	uint32						getLoRID		( );
	uint32						getHiRID		( );

	PakBranch*					findNode		( PakNode* innode );
	PakBranch*					findBranch		( uint32 inlabelcrc );
	PakBranch*					newBranch		( uint32 inlabelcrc, Pool* inp );

	PakFile*					attachFile		( pcstr infilename, Pool* inp );
	bool						detach			( PakNode* innode, Pool* inp );		// node is recycled !!
};








//
// PakNode
//

PakNode::PakNode ( )
{
	prev = next = NULL;
	refcnt = 0;
}


void PakNode::retain ( )
{
	refcnt++;
}


void PakNode::release ( )
{
	SU_ASSERT( refcnt );
	refcnt--;
}








//
// PakFile
//


PakFile* PakFile::create ( pcstr infilename, Pool* inp )
{
	SU_REQUIRE_OR_NULL( inp );
	SU_REQUIRE_OR_NULL( infilename && *infilename );
	SU_REQUIRE_OR_NULL( globals::fs );

	PakFile* pkf = NULL;

	IFileReader* rd = globals::fs->OpenReader( infilename, inp );
	if( rd )
	{
		IPakDesigner::PakHeader hdr;
		if( rd->GetSize() < sizeof(hdr) )
		{
			rd->Release();
			return NULL;
		}
		rd->SetPos( 0 );
		rd->Read( &hdr, sizeof(hdr) );
		if( hdr.magicid==IPakDesigner::PKD_MAGICID && hdr.format==IPakDesigner::PKD_FORMAT )
		{
			uint toc_bs  = hdr.toceo - hdr.tocso;
			uint toc_len = toc_bs / sizeof(IPakDesigner::PakDesc);
			if( toc_len > 0 )
			{
				pkf = inp->New<PakFile>( toc_bs );
				if( pkf )
				{
					pkf->filename.copy( infilename );
					pkf->hdr = hdr;
					pkf->toc = (IPakDesigner::PakDesc*)(pkf+1);
					pkf->toclen = toc_len;

					rd->SetPos( hdr.tocso );
					int readen = rd->Read( pkf->toc, toc_bs );
					SU_ASSERT( readen == toc_bs );

					pkf->fburst = globals::fs->OpenBurst( infilename, inp );
					SU_ASSERT( pkf->fburst );
				}
			}
		}

		rd->Release();
	}

	return pkf;
}


void PakFile::recycle ( Pool* inp )
{
	SafeRelease( fburst );
	inp->Delete( this );
}


pcstr PakFile::getLabel ( )
{
	return hdr.label;
}


uint32 PakFile::getRevision ( )
{
	return hdr.revision;
}


uint32 PakFile::getLoRID ( )
{
	uint r = toc->rid;
	for( uint i=1 ; i<toclen ; i++ )
		r = Min( r, toc[i].rid );
	return r;
}


uint32 PakFile::getHiRID ( )
{
	uint r = toc->rid;
	for( uint i=1 ; i<toclen ; i++ )
		r = Max( r, toc[i].rid );
	return r;
}


bool PakFile::getRsc ( uint32 inRID, PakDesc& outDesc )
{

	IPakDesigner::PakDesc	*desc = toc,
							*last = toc + toclen;

	for( ; desc<last ; desc++ )
	{
		if( desc->rid == inRID )
		{
			outDesc.node = this;
			outDesc.rid = desc->rid;
			outDesc.tid = desc->tid;
			outDesc.dataso = desc->dataso;
			outDesc.dataeo = desc->dataeo;
			outDesc.contentbs = desc->contentbs;
			return true;
		}
	}
	return false;
}


bool PakFile::startTransfer ( TransferParams inParams, TransferParams& outAlignedParams )
{
	void*  a_addr;
	uint64 a_bsize;
	uint64 a_boffset;

	bool res = fburst->StartTransfer( inParams.baddr, inParams.bsize, inParams.boffset, a_addr, a_bsize, a_boffset );
	if( res )
	{
		outAlignedParams.baddr = (byte*) a_addr;
		outAlignedParams.bsize = a_bsize;
		outAlignedParams.boffset = a_boffset;
	}
	return res;
}


void PakFile::abortTransfer ( )
{
	fburst->AbortTransfer();
}


TransferState PakFile::probeTransfer ( )
{
	IFile::State s = fburst->GetState();
	return TransferState(s);
}







//
// PakBranch
//


PakBranch* PakBranch::create ( uint inlabelcrc, Pool* inp )
{
	SU_ASSERT( inp );
	PakBranch* b = inp->New<PakBranch>( 0 );
	SU_REQUIRE_OR_NULL( b );

	b->labelcrc = inlabelcrc;
	b->next = NULL;
	b->prev = NULL;
	b->first = NULL;
	return b;
}


void PakBranch::recycle ( Pool* inp )
{
	SU_ASSERT( inp );
	list_recycle_all( first, inp );
}


bool PakBranch::findRsc ( uint32 inRID, PakDesc& outDesc )
{
	SU_REQUIRE_OR_FALSE( inRID );
	for( PakNode* i=first ; i ; i=i->next ) {
		if( i->getRsc(inRID,outDesc) )
			return true;
	}
	return false;
}


uint32 PakBranch::getLoRID ( )
{
	uint32 r=0;
	for( PakNode* i=first ; i ; i=i->next ) {
		uint32 ri = i->getLoRID();
		if( r==0 || ri<r )	r = ri;
	}
	return r;
}


uint32 PakBranch::getHiRID ( )
{
	uint32 r=0;
	for( PakNode* i=first ; i ; i=i->next ) {
		uint32 ri = i->getHiRID();
		if( r==0 || ri>r )	r = ri;
	}
	return r;
}


bool PakBranch::hasNode ( PakNode* innode )
{
	SU_REQUIRE_OR_FALSE( innode );
	for( PakNode* i=first ; i ; i=i->next )
		if( i==innode )
			return true;
	return false;
}


void PakBranch::insertNode ( PakNode* innode )
{
	SU_ASSERT( innode );
	if( !first )
	{
		list_insert_front( first, innode );
	}
	else
	{
		PakNode *n = first;
		while( n )
		{
			if( innode->getRevision() > n->getRevision() )
			{
				list_insert_inplace( first, innode, n );
				break;
			}
			if( !n->next )
			{
				// at tail !
				list_insert_next( innode, n );
				break;
			}
			n = n->next;
		}
	}
}


void PakBranch::removeNode ( PakNode* innode )
{
	list_remove( first, innode );
}









//
// PakCloud
//


PakCloud::PakCloud( )
{
	first = NULL;
}


void PakCloud::clear( Pool* inp )
{
	list_recycle_all( first, inp );
}


bool PakCloud::findRsc ( uint32 inRID, PakDesc& outDesc )
{
	SU_REQUIRE_OR_FALSE( inRID );
	for( PakBranch* i=first ; i ; i=i->next )
		if( i->findRsc(inRID,outDesc) )
			return true;
	return false;
}


uint32 PakCloud::getLoRID ( )
{
	uint32 r=0;
	for( PakBranch* i=first ; i ; i=i->next ) {
		uint32 ri = i->getLoRID();
		if( r==0 || ri<r )	r = ri;
	}
	return r;
}


uint32 PakCloud::getHiRID ( )
{
	uint32 r=0;
	for( PakBranch* i=first ; i ; i=i->next ) {
		uint32 ri = i->getHiRID();
		if( r==0 || ri>r )	r = ri;
	}
	return r;
}


PakBranch* PakCloud::findNode ( PakNode* innode )
{
	SU_REQUIRE_OR_NULL( innode );
	for( PakBranch* i=first ; i ; i=i->next )
		if( i->hasNode(innode) )
			return i;
	return NULL;
}


PakBranch* PakCloud::findBranch ( uint32 inlabelcrc )
{
	SU_REQUIRE_OR_NULL( inlabelcrc );
	for( PakBranch* i=first ; i ; i=i->next )
		if( i->labelcrc == inlabelcrc )
			return i;
	return NULL;
}


PakBranch* PakCloud::newBranch ( uint32 inlabelcrc, Pool* inp )
{
	PakBranch* b = PakBranch::create( inlabelcrc, inp );
	SU_REQUIRE_OR_NULL( b );
	b->prev = NULL;
	b->next = first;
	first = b;
	return b;
}


PakFile* PakCloud::attachFile( pcstr infilename, Pool* inp )
{
	PakFile* pkf = PakFile::create( infilename, inp );
	SU_REQUIRE_OR_NULL( pkf );

	pcstr label = pkf->getLabel();
	uint32 labelcrc = util::hmac_crc32_w( label );

	PakBranch* b = findBranch( labelcrc );
	if( !b ) {
		b = newBranch( labelcrc, inp );
		SU_ASSERT( b );
	}

	b->insertNode( pkf );
	return pkf;
}


bool PakCloud::detach ( PakNode* innode, Pool* inp )
{
	SU_REQUIRE_OR_FALSE( innode );
	SU_REQUIRE_OR_FALSE( inp );

	PakBranch* b = findNode( innode );
	SU_REQUIRE_OR_FALSE( b );

	if( innode->refcnt > 0 )
		return false;	// still busy !

	b->removeNode( innode );
	innode->recycle( inp );
	return true;
}




