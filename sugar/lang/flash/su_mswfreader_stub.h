/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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



#include <sufw/core/su_core.h>
#include <sufw/codec/su_icodec.h>
#include "su_mswfreader_structs.h"
using namespace su;




namespace
{



	bool inflat_swf ( MCursor& outc, MCursor& inc, swfHEADER& inheader, Pool* p )
	{
		SU_REQUIRE_OR_NULL( p );
		SU_REQUIRE_OR_NULL( inheader.filelength );
		SU_REQUIRE_OR_NULL( inheader.filelength > inc.sizeB() );

		uint  outbs = inheader.filelength+256;
		void* outp  = p->Alloc( inheader.filelength+256 );
		SU_REQUIRE_OR_NULL( outp );

		outc.setup( outp, outbs );
		outc.seek( 8*8 );

		inc.seek( 8*8 );
		bool inflated = ICodec::Convert( outc, inc, p, uid::MCodecZLIB );

		if( inflated )
		{
			inc.seek( 0 );
			outc.seek( 0 );
			util::Memcopy( outc.u8(), inc.u8(), 8 );
			outc.u8[0] = 'F';
		}
		else
		{
			p->Free( outp );
			outc.clear();
		}

		return inflated;
	}




	struct SIDL_STUB_CLASSNAME
	{

		struct Tag
		{
			ISwfReader::TagCode		code;
			int						frame;
			uint64					pos;
			uint64					begin;
			uint64					end;
			uint64					next;
		};

		struct Frame
		{
			int						fromtag;
			int						totag;
			int						labeloffset;
			bool					isanchor;
		};


		Pool*						defpool;

		bool						realized;
		bool						packed;
		Pool*						pool;
		IFileView*					fv;

		MCursor						mc;
		swfHEADER					header;
		vector<Tag>					tags;
		vector<Frame>				frames;
		vector<char>				labels;


		bool Init ( Pool* p, AnyList* o )
		{
			defpool = p;
			realized = false;
			fv = NULL;
			return true;
		}


		void Shut ( )
		{
			_unrealize();
		}


		void _unrealize ( )
		{
			if( realized )
			{
				SU_ASSERT( pool );

				frames.relocate( NULL );
				tags.relocate( NULL );
				labels.relocate( NULL );

				SafeRelease( fv );
				Pool::Destroy( pool );
				realized = false;
			}
		}


		void _parse_tag ( Tag& outtag, uint64 inpos )
		{
			mc.seek( inpos );

			uint codeAndLength = mc.readlsb<uint16>();
			uint code = codeAndLength >> 6;
			uint len  = codeAndLength & 0x3f;
			if( len == 0x3f )
				len = mc.readlsb<uint32>();

			outtag.pos   = inpos;
			outtag.code  = ISwfReader::TagCode( code );
			outtag.begin = mc.pos();
			outtag.end   = mc.pos() + (len<<3);
			outtag.next  = outtag.end;
			outtag.frame = -1;
		}


		int _add_label ( pcstr instr )
		{
			SU_REQUIRE_OR_NEG( labels.pool() );
			SU_REQUIRE_OR_NEG( instr );
			SU_REQUIRE_OR_NEG( instr[0] );

			int ofst = labels.size();
			int slen = util::Strlen(instr);
			labels.resize( ofst+slen+1 );
			util::Strcopy( &labels[ofst], instr, slen );
			return ofst;
		}


		pcstr _label ( int inofst )
		{
			SU_REQUIRE_OR_NULL( inofst>0 );
			SU_ASSERT( inofst < labels.size() );
			return &labels[inofst];
		}


		bool _realize_tags_and_frames ( )
		{
			labels.relocate( pool );
			tags.relocate( pool );
			frames.relocate( pool );

			uint64	p = header.firsttag;
			Tag		t;
			Frame	f;

			f.fromtag     = 0;
			f.totag       = 0;
			f.labeloffset = -1;
			f.isanchor	  = false;

			bool parse_success = true;

			for ( ;; )
			{
				if( p >= header.lasttag )
				{
					parse_success = false;
					break;
				}

				_parse_tag( t, p );

				if( t.code == ISwfReader::End )
					break;

				t.frame = frames.size();
				tags.push_back( t );

				if( t.code==ISwfReader::FrameLabel )
				{
					swfFrameLabelTag ltag;
					ltag.read( mc, t.begin, t.end );
					if( ltag.label.size() )
						f.labeloffset = _add_label( ltag.label );
					f.isanchor = ltag.isanchor;
				}
				else if( t.code==ISwfReader::SetBackgroundColor )
				{
				}
				else if( t.code==ISwfReader::ShowFrame )
				{
					f.totag = tags.size() - 1;
					frames.push_back( f );
					f.fromtag = tags.size();
					f.labeloffset = -1;
					f.isanchor	  = false;
				}

				p = t.next;
			}

			if( frames.size() != header.framecount )
				parse_success = false;

			if( !parse_success )
			{
				labels.relocate( NULL );
				tags.relocate( NULL );
				frames.relocate( NULL );
			}

			return parse_success;
		}


		bool _realize_from_memory ( byte* inBytes, uint inBSize )
		{
			SU_ASSERT( !realized );
			SU_ASSERT( inBytes );
			SU_ASSERT( inBSize );
			SU_ASSERT( inBSize>16 );	// at least hdr+some-packed-bytes
			SU_ASSERT( pool );

			mc.setup( inBytes, inBSize );

			if( !header.read(mc) )
				return false;

			packed = (header.sig[0]=='C');

			if( packed )
			{
				MCursor inflatemc;

				if( !inflat_swf(inflatemc,mc,header,pool) )
					return false;

				if( !header.read(inflatemc) )
					return false;

				// successes, with inflated swf
				SafeRelease( fv );
				mc = inflatemc;
			}

			if( header.filelength > mc.sizeB() )
				return false;

			if( !_realize_tags_and_frames() )
				return false;

			realized = true;
			return true;
		}


		bool Realize ( pcstr inPath, Pool* inPool )
		{
			SU_REQUIRE_OR_FALSE( !realized );
			SU_REQUIRE_OR_FALSE( inPath );
			SU_REQUIRE_OR_FALSE( inPool || defpool );
			SU_REQUIRE_OR_FALSE( globals::fs );

			Pool* rpool = inPool ? inPool : defpool;
			SU_REQUIRE_OR_FALSE( rpool );

			pool = rpool->CreateSub();

			fv = globals::fs->OpenView( inPath, pool );
			if( fv )
			{
				void* fp = fv->Map();
				if( fp )
				{
					if( _realize_from_memory((byte*)fp,fv->GetSize()) )
						return true;
				}
			}

			SafeRelease( fv );
			Pool::Destroy( pool );
			return false;
		}


		bool Realize ( byte* inBytes, uint inBSize, Pool* inPool )
		{
			SU_REQUIRE_OR_FALSE( !realized );
			SU_REQUIRE_OR_FALSE( inBytes );
			SU_REQUIRE_OR_FALSE( inBSize );
			SU_REQUIRE_OR_FALSE( inPool || defpool );

			Pool* rpool = inPool ? inPool : defpool;
			SU_REQUIRE_OR_FALSE( rpool );

			pool = rpool->CreateSub();
			fv = NULL;

			if( _realize_from_memory(inBytes,inBSize) )
				return true;

			Pool::Destroy( pool );
			return false;
		}


		bool IsRealized ( )
		{
			return realized;
		}


		void Unrealize (  )
		{
			_unrealize();
		}


		uint GetByteSize (  )
		{
			SU_REQUIRE_OR_ZERO( realized );
			return header.filelength;
		}


		bool IsPacked (  )
		{
			SU_REQUIRE_OR_FALSE( realized );
			return packed;
		}


		uint GetVersionNumber (  )
		{
			SU_REQUIRE_OR_ZERO( realized );
			return header.version;
		}


		uint GetFrameWidth (  )
		{
			SU_REQUIRE_OR_ZERO( realized );
			return (header.framesize.xmax - header.framesize.xmin);
		}


		uint GetFrameHeight (  )
		{
			SU_REQUIRE_OR_ZERO( realized );
			return (header.framesize.ymax-header.framesize.ymin);
		}


		uint GetFrameRate (  )
		{
			SU_REQUIRE_OR_ZERO( realized );
			return header.framerate;
		}


		MCursor GetCursor ( )
		{
			SU_REQUIRE_OR_RETURN_V( realized, MCursor() );
			return MCursor(mc);
		}


		uint CountTag ( )
		{
			SU_REQUIRE_OR_ZERO( realized );
			return tags.size();
		}


		ISwfReader::Tag GetTag ( uint inIndex )
		{
			ISwfReader::Tag t;
			util::Zero( t );
			SU_REQUIRE_OR_RETURN_V( realized, t );
			SU_REQUIRE_OR_RETURN_V( inIndex<tags.size(), t );

			t.code  = tags[ inIndex ].code;
			t.pos   = tags[ inIndex ].pos;
			t.begin = tags[ inIndex ].begin;
			t.end   = tags[ inIndex ].end;
			t.next  = tags[ inIndex ].next;
			return t;
		}


		ISwfReader::Tag GetFirstTag (  )
		{
			ISwfReader::Tag t;
			util::Zero( t );
			SU_REQUIRE_OR_RETURN_V( realized, t );

			return GetTag( 0 );
		}


		ISwfReader::Tag GetLastTag (  )
		{
			ISwfReader::Tag t;
			util::Zero( t );
			SU_REQUIRE_OR_RETURN_V( realized, t );
			SU_REQUIRE_OR_RETURN_V( CountTag()>0, t );

			return GetTag( CountTag()-1 );
		}


		uint CountFrame ( )
		{
			SU_REQUIRE_OR_ZERO( realized );
		
			return frames.size();
		}


		ISwfReader::Frame GetFrame ( uint inIndex )
		{
			ISwfReader::Frame f;
			util::Zero( f );
			SU_REQUIRE_OR_RETURN_V( realized, f );
			SU_REQUIRE_OR_RETURN_V( inIndex<frames.size(), f );

			f.fromTag  = frames[ inIndex ].fromtag;
			f.toTag    = frames[ inIndex ].totag;
			f.isAnchor = frames[ inIndex ].isanchor;
			f.label    = _label( frames[ inIndex ].labeloffset );
			return f;
		}


	};


}


