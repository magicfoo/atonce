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



#include "su_asset.h"
using namespace su;






namespace
{


	struct SIDL_STUB_CLASSNAME
	{

		Pool*							pool;
		PString							filename;

		IPakDesigner::PakHeader			pheader;
		vector<IPakDesigner::PakDesc>	pdesc;

		bool							updating;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			pdesc.relocate( p );
			util::Zero( pheader );
			updating = false;
			return true;
		}


		void Shut ( )
		{
			//
		}


		void assert_integrity ( )
		{
			SU_ASSERT( !updating );

			uint32 pak_bs;
			IFileReader* rd = globals::fs->OpenReader( filename, pool );
			SU_ASSERT( rd );
			pak_bs = rd->GetSize();
			SafeRelease( rd );

			SU_ASSERT( pheader.magicid == IPakDesigner::PKD_MAGICID );
			SU_ASSERT( pheader.format == IPakDesigner::PKD_FORMAT );
			SU_ASSERT( pheader.fullbs == pak_bs );
			SU_ASSERT( util::Strlen(pheader.label) < sizeof(pheader.label) );
			SU_ASSERT( util::Strlen(pheader.comment) < sizeof(pheader.comment) );

			uint hdr_bs = sizeof(pheader);
			SU_ASSERT( pak_bs >= hdr_bs );

			uint toc_bs = pheader.toceo - pheader.tocso;
			SU_ASSERT( (toc_bs%sizeof(IPakDesigner::PakDesc)) == 0 );
			uint rcnt = toc_bs / sizeof(IPakDesigner::PakDesc);
			SU_ASSERT( pdesc.size() == rcnt );

			uint32 lo_rid=0, hi_rid=0;

			for( uint i=0 ; i<pdesc.size() ; i++ )
			{
				IPakDesigner::PakDesc& desc = pdesc[i];

				SU_ASSERT( desc.rid );
				SU_ASSERT( desc.tid );

				if( i==0 )
				{
					lo_rid = desc.rid;
					hi_rid = desc.rid;
				}
				else
				{
					SU_ASSERT( pdesc[i].rid > pdesc[i-1].rid );
					hi_rid = desc.rid;
				}

				SU_ASSERT( desc.dataeo >  desc.dataso );
				SU_ASSERT( desc.dataso <= pak_bs );
				SU_ASSERT( desc.dataeo <= pak_bs );
				SU_ASSERT( desc.contentbs >= (desc.dataeo-desc.dataso) );
			}

			SU_ASSERT( pheader.lorid == lo_rid );
			SU_ASSERT( pheader.hirid == hi_rid );
		}


		void sync_update ( )
		{
			SU_ASSERT( !updating );

			IFileWriter* wr = globals::fs->OpenWriter( filename, pool, false, true );
			SU_ASSERT( wr );

			pheader.lorid = 0;
			pheader.hirid = 0;

			// update descriptors

			for( uint i=0 ; i<pdesc.size() ; i++ )
			{
				IPakDesigner::PakDesc& desc = pdesc[i];

				if( i==0 )
				{
					pheader.lorid = desc.rid;
					pheader.hirid = desc.rid;
				}
				else
				{
					SU_ASSERT( pdesc[i].rid > pdesc[i-1].rid );
					pheader.hirid = desc.rid;
				}
			}

			// write-back the TOC

			wr->SetPos( wr->GetSize() );
			pheader.tocso = wr->GetPos();
			wr->Write( pdesc.data(), pdesc.bsize() );
			pheader.toceo = wr->GetPos();
			pheader.fullbs = wr->GetPos();

			// write-back the header

			wr->SetPos( 0 );
			wr->Write( &pheader, sizeof(pheader) );

			wr->Release();
		}


		int find_desc_by_rid ( uint32 inRID )
		{
			for( int i=0 ; i<pdesc.size() ; i++ )
			{
				if( pdesc[i].rid == inRID )
					return i;
			}
			return -1;
		}


		int insert_desc_by_rid ( uint32 inRID )
		{
			int i;

			if( pdesc.empty() || pdesc.back().rid < inRID )
			{
				// insert front
				i = pdesc.size();
				pdesc.push_back( IPakDesigner::PakDesc() );
			}
			else if( pdesc.front().rid > inRID )
			{
				// insert back
				pdesc.insert( pdesc.begin() );
				i = 0;
			}
			else
			{
				// insert ith
				for( i=0 ; i<pdesc.size() ; i++ )
				{
					if( pdesc[i].rid == inRID )
						break;
					if( pdesc[i].rid > inRID ) {
						pdesc.insert( pdesc.begin()+i );
						break;
					}
				}
			}

			return i;
		}


		bool generate_new_pak ( pcstr inFilename )
		{
			SU_REQUIRE_OR_FALSE( globals::fs );

			util::Zero( pheader );

			pheader.magicid = IPakDesigner::PKD_MAGICID;
			pheader.format = IPakDesigner::PKD_FORMAT;
			pheader.fullbs = sizeof(pheader);
			pheader.tocso = sizeof(pheader);
			pheader.toceo = sizeof(pheader);

			pdesc.clear();
			updating = false;

			IFileWriter* wr = globals::fs->OpenWriter( inFilename, pool );
			SU_REQUIRE_OR_FALSE( wr );
			wr->SetPos( 0 );
			wr->Write( &pheader, sizeof(pheader) );
			wr->Release();

			filename.copy( inFilename );
			assert_integrity();
			return true;
		}


		bool load_pak ( IFileReader* inrd, pcstr inFilename )
		{
			SU_REQUIRE_OR_FALSE( globals::fs );
			SU_REQUIRE_OR_FALSE( inrd );
			SU_REQUIRE_OR_FALSE( inrd->GetSize() >= sizeof(pheader) );

			// load header
			inrd->SetPos( 0 );
			inrd->Read( &pheader, sizeof(pheader) );
			if( pheader.magicid==IPakDesigner::PKD_MAGICID && pheader.format==IPakDesigner::PKD_FORMAT )
			{
				uint toc_bs  = pheader.toceo - pheader.tocso;
				SU_ASSERT( (toc_bs % sizeof(IPakDesigner::PakDesc)) == 0 );
				uint rcnt = toc_bs / sizeof(IPakDesigner::PakDesc);

				// load toc
				inrd->SetPos( pheader.tocso );
				pdesc.resize( rcnt );
				inrd->Read( pdesc.data(), toc_bs );

				filename.copy( inFilename );
				assert_integrity();
				return true;
			}
			return false;
		}


		bool Setup ( pcstr inFilename, bool inForceCreate )
		{
			SU_REQUIRE_OR_FALSE( globals::fs );

			if( updating )
				return false;	// busy !!

			if( inForceCreate || !globals::fs->IsFile(inFilename) )
			{
				return generate_new_pak(inFilename);
			}
			else
			{
				// open it
				IFileReader* rd = globals::fs->OpenReader( inFilename, pool );
				if( !rd )
					return false;

				// empty file => generate it
				if( rd->GetSize() < sizeof(pheader) )
				{
					rd->Release();
					return generate_new_pak(inFilename);
				}
				else
				{
					// load file
					bool ret = load_pak(rd,inFilename);
					rd->Release();
					return ret;
				}
			}
		}


		IPakDesigner::PakHeader GetHeader (  )
		{
			if( updating || filename.empty() )
			{
				IPakDesigner::PakHeader null_h;
				util::Zero( null_h );
				return null_h;
			}
			return pheader;
		}


		IPakDesigner::PakDesc GetDesc ( uint32 inRID )
		{
			int i = find_desc_by_rid( inRID );
			if( updating || i<0 )
			{
				IPakDesigner::PakDesc null_d;
				util::Zero( null_d );
				return null_d;
			}
			return pdesc[i];
		}


		uint CountDesc ( )
		{
			SU_REQUIRE_OR_0( !updating );
			return pdesc.size();
		}


		IPakDesigner::PakDesc EnumDesc ( int inIndex )
		{
			if( updating || inIndex<0 || inIndex>=pdesc.size() )
			{
				IPakDesigner::PakDesc null_d;
				util::Zero( null_d );
				return null_d;
			}
			return pdesc[inIndex];
		}


		bool ExtractBytes ( uint32 inStartOffset, uint32 inEndOffset, void* inDataAddr )
		{
			SU_REQUIRE_OR_FALSE( !updating );
			SU_REQUIRE_OR_FALSE( globals::fs );
			SU_REQUIRE_OR_FALSE( inDataAddr );
			SU_REQUIRE_OR_FALSE( inEndOffset > inStartOffset );
			int databs = inEndOffset - inStartOffset;
			int readen = 0;
			IFileReader* rd = globals::fs->OpenReader( filename, pool );
			if( rd )
			{
				if( inEndOffset <= rd->GetSize() )
				{
					rd->SetPos( inStartOffset );
					readen = rd->Read( inDataAddr, databs );
				}
				rd->Release();
			}
			return (readen == databs);
		}

		
		bool ExtractRaw ( uint32 inRID, void* inDataAddr )
		{
			SU_REQUIRE_OR_FALSE( !updating );
			SU_REQUIRE_OR_FALSE( inDataAddr );

			int i = find_desc_by_rid( inRID );
			SU_REQUIRE_OR_FALSE( i>=0 );

			return ExtractBytes( pdesc[i].dataso, pdesc[i].dataeo, inDataAddr );
		}


		bool ExtractContent ( uint32 inRID, void* inDataAddr )
		{
			SU_REQUIRE_OR_FALSE( !updating );
			SU_REQUIRE_OR_FALSE( inDataAddr );

			int i = find_desc_by_rid( inRID );
			SU_REQUIRE_OR_FALSE( i>=0 );

			if( !ExtractBytes(pdesc[i].dataso,pdesc[i].dataeo,inDataAddr) )
				return false;

			uint databs = pdesc[i].dataeo - pdesc[i].dataso;
			uint contentbs = pdesc[i].contentbs;
			SU_ASSERT( contentbs >= databs );
			if( contentbs > databs )
			{
				// lzo unpack !
			}

			return true;
		}


		bool StartUpdating (  )
		{
			SU_REQUIRE_OR_FALSE( filename.size() );
			SU_REQUIRE_OR_FALSE( !updating );
			updating = true;
			return true;
		}


		bool UpdateRevision ( uint inRevision )
		{
			SU_REQUIRE_OR_FALSE( filename.size() );
			SU_REQUIRE_OR_FALSE( updating );
			pheader.revision = inRevision;
			return true;
		}


		bool UpdateLabel ( pcstr inLabel )
		{
			SU_REQUIRE_OR_FALSE( filename.size() );
			SU_REQUIRE_OR_FALSE( updating );
			SU_REQUIRE_OR_FALSE( inLabel && *inLabel );
			SU_REQUIRE_OR_FALSE( util::Strlen(inLabel) < sizeof(pheader.label) );

			util::Zero( pheader.label );
			util::Strcopy( pheader.label, inLabel );
			return true;
		}


		bool UpdateComment ( pcstr inComment )
		{
			SU_REQUIRE_OR_FALSE( filename.size() );
			SU_REQUIRE_OR_FALSE( updating );
			SU_REQUIRE_OR_FALSE( inComment && *inComment );
			SU_REQUIRE_OR_FALSE( util::Strlen(inComment) < sizeof(pheader.comment) );

			util::Zero( pheader.comment );
			util::Strcopy( pheader.comment, inComment );
			return true;
		}


		bool UpdateContent ( uint32 inRID, uint32 inTID, void* inDataAddr, uint32 inDataBSize, bool inPackable )
		{
			SU_REQUIRE_OR_FALSE( filename.size() );
			SU_REQUIRE_OR_FALSE( updating );
			SU_REQUIRE_OR_FALSE( inRID );
			SU_REQUIRE_OR_FALSE( inTID );
			SU_REQUIRE_OR_FALSE( inDataAddr );
			SU_REQUIRE_OR_FALSE( inDataBSize );

			IFileWriter* wr = globals::fs->OpenWriter( filename, pool, false, true );
			SU_REQUIRE_OR_FALSE( wr );

			int i = insert_desc_by_rid( inRID );

			pdesc[i].rid = inRID;
			pdesc[i].tid = inTID;
			pdesc[i].dataso = wr->GetSize();
			pdesc[i].dataeo = pdesc[i].dataso + inDataBSize;
			pdesc[i].contentbs = inDataBSize;

			wr->SetPos( pdesc[i].dataso );
			wr->Write( inDataAddr, inDataBSize );

			SafeRelease( wr );
			return true;
		}


		void EndUpdating (  )
		{
			SU_REQUIRE_OR_RETURN( filename.size() );
			SU_REQUIRE_OR_RETURN( updating );
			updating = false;
			sync_update();
			assert_integrity();
		}


		bool Optimize (  )
		{
			SU_REQUIRE_OR_FALSE( filename.size() );
			SU_REQUIRE_OR_FALSE( !updating );
			return false;
		}


	};


}


