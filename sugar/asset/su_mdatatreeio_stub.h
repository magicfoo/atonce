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


#include <yaml.h>
using namespace su;



#define MAGICGID_PNAME	"io_mid"
#define REFCOUNT_PNAME	"io_refc"
#define EMITCOUNT_PNAME	"io_emitc"
#define ANCHOR_PNAME	"anchor"



namespace
{
	Pool* g_fatherPool = NULL;


	pcstr get_sprop ( IDatatree* dtt, int idx, pcstr pname )
	{
		pcstr p = NULL;
		dtt->pushproperty( idx );
		if( dtt->ismapping(-1) )
		{
			dtt->push( -1, pname );
			p = dtt->asstring( -1 );
			dtt->pop( 2 );
		} else {
			dtt->pop( 1 );
		}
		return p;
	}


	int get_iprop ( IDatatree* dtt, int idx, pcstr pname )
	{
		int p = -1;
		dtt->pushproperty( idx );
		if( dtt->ismapping(-1) )
		{
			dtt->push( -1, pname );
			p = dtt->asinteger( -1 );
			dtt->pop( 2 );
		} else {
			dtt->pop( 1 );
		}
		return p;
	}


	bool set_iprop ( IDatatree* dtt, int idx, pcstr pname, int pvalue )
	{
		int bidx = dtt->baseindex( idx );
		dtt->pushproperty(bidx);
		if( dtt->isnil(-1) )
		{
			dtt->pop( 1 );
			dtt->pushmapping();
			dtt->pushinteger( pvalue );
			dtt->store( -2, pname );
			dtt->storeproperty(bidx);
			return true;
		}
		else if( dtt->ismapping(-1) )
		{
			dtt->pushinteger( pvalue );
			dtt->store( -2, pname );
			dtt->pop( 1 );
			return true;
		}
		else
		{
			dtt->pop( 1 );
			return false;
		}
	}


	bool set_sprop ( IDatatree* dtt, int idx, pcstr pname, pcstr pvalue )
	{
		int bidx = dtt->baseindex( idx );
		dtt->pushproperty(bidx);
		if( dtt->isnil(-1) )
		{
			dtt->pop( 1 );
			dtt->pushmapping();
			if( pvalue && *pvalue )		dtt->pushstring( pvalue );
			else						dtt->pushnil();
			dtt->store( -2, pname );
			dtt->storeproperty(bidx);
			return true;
		}
		else if( dtt->ismapping(-1) )
		{
			if( pvalue && *pvalue )		dtt->pushstring( pvalue );
			else						dtt->pushnil();
			dtt->store( -2, pname );
			dtt->pop( 1 );
			return true;
		}
		else
		{
			dtt->pop( 1 );
			return false;
		}
	}


	int get_magicid ( )
	{
		static int _mid = 0;
		_mid = (_mid+1) & 0xFFFFFFF;
		return _mid;
	}


	bool parse_event ( yaml_parser_t* pr, yaml_event_t& e, yaml_event_type_t break_on=YAML_NO_EVENT )
	{
		SU_REQUIRE_OR_FALSE( pr->error == YAML_NO_ERROR );
		yaml_event_delete(&e);
		int r = yaml_parser_parse( pr, &e );
		if( r == 0 )
			return false;
		if( e.type == break_on)
			return false;
		return true;
	}


	bool parse_event ( yaml_parser_t* pr, yaml_event_t& e1, yaml_event_t& e2, yaml_event_type_t break_on=YAML_NO_EVENT )
	{
		return parse_event(pr,e1,break_on) && parse_event(pr,e2,break_on);
	}


	pcstr event_style ( yaml_event_t& e )
	{
		if( e.type == YAML_SCALAR_EVENT )
		{
			yaml_scalar_style_t s = e.data.scalar.style;
			if( s==YAML_PLAIN_SCALAR_STYLE )			return "plain";
			if( s==YAML_SINGLE_QUOTED_SCALAR_STYLE )	return "squoted";
			if( s==YAML_DOUBLE_QUOTED_SCALAR_STYLE )	return "dquoted";
			if( s==YAML_LITERAL_SCALAR_STYLE )			return "literal";
			if( s==YAML_FOLDED_SCALAR_STYLE )			return "folded";
		}
		else if( e.type == YAML_SEQUENCE_START_EVENT )
		{
			yaml_sequence_style_t s = e.data.sequence_start.style;
			if( s==YAML_BLOCK_SEQUENCE_STYLE )			return "block";
			if( s==YAML_FLOW_SEQUENCE_STYLE )			return "flow";
		}
		else if( e.type == YAML_MAPPING_START_EVENT )
		{
			yaml_mapping_style_t s = e.data.mapping_start.style;
			if( s==YAML_BLOCK_MAPPING_STYLE )			return "block";
			if( s==YAML_FLOW_MAPPING_STYLE )			return "flow";
		}
		return "any";
	}


	char* event_tag ( yaml_event_t& e )
	{
		if( e.type == YAML_SCALAR_EVENT )
		{
			return (char*) e.data.scalar.tag;
		}
		else if( e.type == YAML_SEQUENCE_START_EVENT )
		{
			return (char*) e.data.sequence_start.tag;
		}
		else if( e.type == YAML_MAPPING_START_EVENT )
		{
			return (char*) e.data.mapping_start.tag;
		}
		return NULL;
	}


	char* event_anchor ( yaml_event_t& e )
	{
		if( e.type == YAML_SCALAR_EVENT )
		{
			return (char*) e.data.scalar.anchor;
		}
		else if( e.type == YAML_ALIAS_EVENT )
		{
			return (char*) e.data.alias.anchor;
		}
		else if( e.type == YAML_SEQUENCE_START_EVENT )
		{
			return (char*) e.data.sequence_start.anchor;
		}
		else if( e.type == YAML_MAPPING_START_EVENT )
		{
			return (char*) e.data.mapping_start.anchor;
		}
		return NULL;
	}


	LString event_statement ( yaml_event_t& e )
	{
		char stt[ 6*16 ];
		util::Strformat( stt, "%d,%d,%d-%d,%d,%d",
			e.start_mark.index, e.start_mark.line, e.start_mark.column,
			e.end_mark.index, e.end_mark.line, e.end_mark.column	);
		return LString(stt);
	}


	Pool* uu_pool ( )
	{
		SU_ASSERT( g_fatherPool );
		Pool* p = g_fatherPool->CreateSub();
		SU_ASSERT( p );
		return p;
	}


	template <typename T> void uu_encode ( T* inarray, uint inlen, char*& outaddr, Pool* pool )
	{
		SU_ASSERT( inarray );
		SU_ASSERT( inlen );
		SU_ASSERT( pool );

		uint uulen;
		char* uudata;
		uulen = util::uuencodelen( inarray, sizeof(T)*inlen );
		uudata = (char*) pool->Alloc( uulen+1 );
		SU_ASSERT( uudata );

		util::Zero( uudata, uulen+1 );
		util::uuencode( uudata, inarray, sizeof(T)*inlen );

		outaddr = uudata;
	}


	template <typename T> void uu_decode ( yaml_event_t& pe, T*& outaddr, uint& outlen, Pool* pool )
	{
		SU_ASSERT( pool );

		pcstr s0 = (pcstr) pe.data.scalar.value;
		pcstr s1 = s0 + pe.data.scalar.length;
		SU_ASSERT( s0 && s1 );

		SU_ASSERT( util::uustream(s0,s1) );
		uint  blen = util::uudecodelen( s0, s1 );
		void* bdata	= pool->Alloc( blen );
		SU_ASSERT( bdata );
		SU_ASSERT( (blen%sizeof(T))==0 );

		uint dlen = util::uudecode( bdata, s0, s1 );
		SU_ASSERT( dlen == blen );

		outaddr = (T*)bdata;
		outlen   = blen/sizeof(T);
	}


	template <typename T> void uu_encodeseq ( T* inarray, uint inlen, char*& outaddr, Pool* pool )
	{
		SU_ASSERT( inarray );
		SU_ASSERT( inlen );
		SU_ASSERT( pool );

		uint uulen;
		char* uudata;
		uulen = util::uuencodeseqlen( util::UUSEQ_MIXED, inarray, inlen );
		uudata = (char*) pool->Alloc( uulen+1 );
		SU_ASSERT( uudata );

		util::Zero( uudata, uulen+1 );
		util::uuencodeseq( uudata, util::UUSEQ_MIXED, inarray, inlen );

		outaddr = uudata;
	}


	template <typename T> void uu_decodeseq ( yaml_event_t& pe, T*& outaddr, uint& outlen, Pool* pool )
	{
		SU_ASSERT( pool );

		pcstr s0 = (pcstr) pe.data.scalar.value;
		pcstr s1 = s0 + pe.data.scalar.length;
		SU_ASSERT( s0 && s1 );

		uint n = util::uudecodeseqlen( s0, s1 );
		T* seq = (T*)pool->Alloc( sizeof(T)*n );
		SU_ASSERT( seq );

		uint m = util::uudecodeseq( seq, s0, s1 );
		SU_ASSERT( m == n );

		outaddr = seq;
		outlen  = n;
	}



	struct str_tok
	{
		pcstr t0, t1, e;

		str_tok( pcstr s0, pcstr s1 )
		{
			t0 = t1 = s0;
			e = s1;
		}

		bool next ( )
		{
			while( (t1<e) && (*t1<=32) )	t1++;
			t0 = t1;
			while( (t1<e) && (*t1>32) )		t1++;
			return (t1>t0);
		}
	};







	void parse_element ( yaml_parser_t* pr, yaml_event_t& pe, IDatatree* dtt, int anchormap )
	{
		SU_REQUIRE_OR_RETURN( pr->error == 0 );

		LString style, tag, anchor, stt;
		style  = event_style( pe );
		tag    = event_tag( pe );
		anchor = event_anchor( pe );
		stt	   = event_statement( pe );

		switch( pe.type )
		{
			case YAML_SCALAR_EVENT :
			{
				if( tag.equals("bin") )
				{
					// blob as a blob
					Pool* p = uu_pool();
					byte* uuaddr; uint uulen;
					uu_decode( pe, uuaddr, uulen, p );
					dtt->pushblob( uuaddr, uulen );
					p->Release();
				}
				else if( tag.equals("bini") )
				{
					// blob as a sequence of signed-integers
					Pool* p = uu_pool();
					int* uuaddr; uint uulen;
					uu_decode( pe, uuaddr, uulen, p );
					dtt->pushsequence( uulen );
					for( int i=0 ; i<uulen ; i++ )
					{
						dtt->pushinteger( uuaddr[i] );
						dtt->append( -2 );
					}
					p->Release();
				}
				else if( tag.equals("binf") )
				{
					// blob as a sequence of floats
					Pool* p = uu_pool();
					float* uuaddr; uint uulen;
					uu_decode( pe, uuaddr, uulen, p );
					dtt->pushsequence( uulen );
					for( int i=0 ; i<uulen ; i++ )
					{
						dtt->pushnumber( uuaddr[i] );
						dtt->append( -2 );
					}
					p->Release();
				}
				else if( tag.equals("seq") )
				{
					// string as a sequence of string(split by spaces)
					pcstr s0 = (pcstr) pe.data.scalar.value;
					pcstr s1 = s0 + pe.data.scalar.length;
					SU_ASSERT( s0 && s1 );
					int n = 0;
					str_tok tok(s0,s1);
					while( tok.next() )	n++;
					dtt->pushsequence( n );
					tok = str_tok(s0,s1);
					for( int i=0 ; tok.next() ; i++ )
						dtt->pushstring( tok.t0, tok.t1 );
				}
				else if( tag.equals("seqi") )
				{
					// string as a sequence of signed-integers
					Pool* p = uu_pool();
					int* seq; uint n;
					uu_decodeseq( pe, seq, n, p );
					dtt->pushsequence( n );
					for( int i=0 ; i<n ; i++ )
					{
						dtt->pushinteger( seq[i] );
						dtt->append( -2 );
					}
					p->Release();
				}
				else if( tag.equals("seqf") )
				{
					// string as a sequence of floats
					Pool* p = uu_pool();
					float* seq; uint n;
					uu_decodeseq( pe, seq, n, p );
					dtt->pushsequence( n );
					for( int i=0 ; i<n ; i++ )
					{
						dtt->pushnumber( seq[i] );
						dtt->append( -2 );
					}
					p->Release();
				}
				else if( tag.equals("str") )
				{
					// string as a sequence of floats
					dtt->pushstring( (pcstr)pe.data.scalar.value );
				}
				else
				{
					// string as a generic scalar
					dtt->pushscalar( (pcstr)pe.data.scalar.value );
				}

				if( anchor.size() )
				{
					dtt->push( -1 );
					dtt->store( anchormap, anchor.cstr() );
				}
				break;
			}

			case YAML_ALIAS_EVENT :
			{
				SU_ASSERT( anchor.size() );
				dtt->push( anchormap, anchor.cstr() );
				SU_ASSERT( !dtt->isnil(-1) );
				break;
			}

			case YAML_SEQUENCE_START_EVENT :
			{
				dtt->pushsequence();

				if( anchor.size() )
				{
					dtt->push( -1 );
					dtt->store( anchormap, anchor.cstr() );
				}

				yaml_event_t e;
				util::Zero(e);

				while( parse_event(pr,e,YAML_SEQUENCE_END_EVENT) )
				{
					parse_element( pr, e, dtt, anchormap );
					dtt->append( -2 );
				}

				yaml_event_delete( &e );
				break;
			}

			case YAML_MAPPING_START_EVENT :
			{
				dtt->pushmapping();

				if( anchor.size() )
				{
					dtt->push( -1 );
					dtt->store( anchormap, anchor.cstr() );
				}

				yaml_event_t ek, ev;
				util::Zero(ek);
				util::Zero(ev);

				while( parse_event(pr,ek,ev,YAML_MAPPING_END_EVENT) )
				{
					parse_element( pr, ek, dtt, anchormap );
					parse_element( pr, ev, dtt, anchormap );
					dtt->store( -3, dtt->tostring(-2) );
					dtt->pop(1);
				}

				yaml_event_delete( &ek );
				yaml_event_delete( &ev );
				break;
			}
		}

		if( style.size() )		set_sprop( dtt, -1, "style", style.str() );
		if( tag.size() )		set_sprop( dtt, -1, "tag", tag.str() );
		if( stt.size() )		set_sprop( dtt, -1, "statement", stt.str() );
	}


	void parse_document ( yaml_parser_t* pr, IDatatree* dtt )
	{
		yaml_event_t e;
		util::Zero(e);

		// doc's anchor map
		dtt->pushmapping();
		int anchormap = dtt->gettop();

		while( parse_event(pr,e,YAML_DOCUMENT_END_EVENT) )
			parse_element( pr, e, dtt, anchormap );

		dtt->remove( anchormap );

		yaml_event_delete( &e );
	}


	int parse_input ( yaml_parser_t* pr, IDatatree* dtt )
	{
	//	yaml_parser_set_encoding( &parser, YAML_ANY_ENCODING );

		int count = 0;

		yaml_event_t e;
		util::Zero(e);

		while( parse_event(pr,e,YAML_STREAM_END_EVENT) )
		{
			if( e.type==YAML_STREAM_START_EVENT )
			{
				//
			}
			else if( e.type==YAML_DOCUMENT_START_EVENT )
			{
				parse_document( pr, dtt );
				count++;
			}
			else
			{
				// unexpected event !
				// just skip it ?
			}
		}

		yaml_event_delete( &e );
		return count;
	}



	yaml_scalar_style_t scalar_style ( pcstr s )
	{
		if( util::Strequals(s,"plain") )			return YAML_PLAIN_SCALAR_STYLE;
		if( util::Strequals(s,"squoted") )			return YAML_SINGLE_QUOTED_SCALAR_STYLE;
		if( util::Strequals(s,"dquoted") )			return YAML_DOUBLE_QUOTED_SCALAR_STYLE;
		if( util::Strequals(s,"literal") )			return YAML_LITERAL_SCALAR_STYLE;
		if( util::Strequals(s,"folded") )			return YAML_FOLDED_SCALAR_STYLE;
		return YAML_ANY_SCALAR_STYLE;
	}


	yaml_sequence_style_t sequence_style ( pcstr s )
	{
		if( util::Strequals(s,"block") )			return YAML_BLOCK_SEQUENCE_STYLE;
		if( util::Strequals(s,"flow") )			return YAML_FLOW_SEQUENCE_STYLE;
		return YAML_ANY_SEQUENCE_STYLE;
	}


	yaml_mapping_style_t mapping_style ( pcstr s )
	{
		if( util::Strequals(s,"block") )			return YAML_BLOCK_MAPPING_STYLE;
		if( util::Strequals(s,"flow") )			return YAML_FLOW_MAPPING_STYLE;
		return YAML_ANY_MAPPING_STYLE;
	}


	yaml_char_t* ys ( const char* s )
	{
		return (yaml_char_t*)s;
	}


	void emit_key( yaml_emitter_t* em, pcstr k )
	{
		yaml_event_t e;
		yaml_scalar_event_initialize( &e, NULL, NULL, ys(k), util::Strlen(k), 1, 1, YAML_ANY_SCALAR_STYLE );
		yaml_emitter_emit( em, &e );
	}


	void emit_element( yaml_emitter_t* em, IDatatree* dtt, int mid )
	{
		SU_REQUIRE_OR_RETURN( em->error == 0 );

		pcstr tag    = get_sprop(dtt,-1,"tag");
		pcstr style  = get_sprop(dtt,-1,"style");
		int   refc	 = get_iprop(dtt,-1,REFCOUNT_PNAME);
		int   emitc	 = get_iprop(dtt,-1,EMITCOUNT_PNAME);
		pcstr anchor = refc>1 ? get_sprop(dtt,-1,ANCHOR_PNAME) : NULL;

		// emit the anchor ?
		// count the number of emitting for this element
		// if emitted more than one time => must be referenced by an anchor

		if( anchor )
		{
			// #emit++ stored in property
			set_iprop(dtt,-1,EMITCOUNT_PNAME,emitc+1);

			if( emitc>0 )
			{
				// emit an alias to the previously emitted anchor
				yaml_event_t e;
				yaml_alias_event_initialize( &e, ys(anchor) );
				yaml_emitter_emit( em, &e );
				return;
			}
		}

		// else emit the value ...

		if( dtt->isscalar(-1) )
		{
			IDatatree::ScalarType st;
			st = dtt->scalartype(-1);
			SU_ASSERT( st != IDatatree::ST_Undef );

			if( st == IDatatree::ST_Blob )
			{
				Pool* p = uu_pool();
				SU_ASSERT( p );

				int blen;
				byte* bdata;
				bdata = dtt->asblob( -1, &blen );
				SU_ASSERT( bdata );

				char* uudata;
				uu_encode( bdata, blen, uudata, p );

				yaml_event_t e;
				yaml_scalar_event_initialize( &e, ys(anchor), ys("bin"), ys(uudata), util::Strlen(uudata), 0, 0, scalar_style(style) );
				yaml_emitter_emit( em, &e );

				p->Release();
			}
			else
			{
				pcstr content = dtt->tostring(-1);

				yaml_event_t e;
				yaml_scalar_event_initialize( &e, ys(anchor), ys(tag), ys(content), util::Strlen(content), tag?0:1, tag?0:1, scalar_style(style) );
				yaml_emitter_emit( em, &e );
			}
		}
		else if( dtt->issequence(-1) )
		{
			if( util::Strequals(tag,"bini") )
			{
				Pool* p = uu_pool();
				SU_ASSERT( p );

				int n = dtt->size(-1);
				int* arr = p->Array<int>( n );
				SU_ASSERT( arr );
				for( int i=0 ; i<n ; i++ )
				{
					dtt->push( -1, i );
					arr[i] = dtt->tointeger(-1);
					dtt->pop( 1 );
				}

				char* uudata;
				uu_encode( arr, n, uudata, p );

				yaml_event_t e;
				yaml_scalar_event_initialize( &e, ys(anchor), ys("bini"), ys(uudata), util::Strlen(uudata), 0, 0, scalar_style(style) );
				yaml_emitter_emit( em, &e );

				p->Release();
			}
			else if( util::Strequals(tag,"binf") )
			{
				Pool* p = uu_pool();
				SU_ASSERT( p );

				int n = dtt->size(-1);
				float* arr = p->Array<float>( n );
				SU_ASSERT( arr );
				for( int i=0 ; i<n ; i++ )
				{
					dtt->push( -1, i );
					arr[i] = dtt->tonumber(-1);
					dtt->pop( 1 );
				}

				char* uudata;
				uu_encode( arr, n, uudata, p );

				yaml_event_t e;
				yaml_scalar_event_initialize( &e, ys(anchor), ys("binf"), ys(uudata), util::Strlen(uudata), 0, 0, scalar_style(style) );
				yaml_emitter_emit( em, &e );

				p->Release();
			}
			else if( util::Strequals(tag,"seqi") )
			{
				Pool* p = uu_pool();
				SU_ASSERT( p );

				int n = dtt->size(-1);
				int* arr = p->Array<int>( n );
				SU_ASSERT( arr );
				for( int i=0 ; i<n ; i++ )
				{
					dtt->push( -1, i );
					arr[i] = dtt->tointeger(-1);
					dtt->pop( 1 );
				}

				char* uudata;
				uu_encodeseq( arr, n, uudata, p );

				yaml_event_t e;
				yaml_scalar_event_initialize( &e, ys(anchor), ys("seqi"), ys(uudata), util::Strlen(uudata), 0, 0, scalar_style(style) );
				yaml_emitter_emit( em, &e );

				p->Release();
			}
			else if( util::Strequals(tag,"seqf") )
			{
				Pool* p = uu_pool();
				SU_ASSERT( p );

				int n = dtt->size(-1);
				float* arr = p->Array<float>( n );
				SU_ASSERT( arr );
				for( int i=0 ; i<n ; i++ )
				{
					dtt->push( -1, i );
					arr[i] = dtt->tonumber(-1);
					dtt->pop( 1 );
				}

				char* uudata;
				uu_encodeseq( arr, n, uudata, p );

				yaml_event_t e;
				yaml_scalar_event_initialize( &e, ys(anchor), ys("seqf"), ys(uudata), util::Strlen(uudata), 0, 0, scalar_style(style) );
				yaml_emitter_emit( em, &e );

				p->Release();
			}
			else
			{
				yaml_event_t e;
				yaml_sequence_start_event_initialize( &e, ys(anchor), ys(tag), tag?0:1, sequence_style(style) );
				yaml_emitter_emit( em, &e );

				int n = dtt->size(-1);
				for( int i=0 ; i<n ; i++ )
				{
					dtt->push( -1, i );
					emit_element( em, dtt, mid );
					dtt->pop( 1 );
				}

				yaml_sequence_end_event_initialize( &e );
				yaml_emitter_emit( em, &e );
			}
		}
		else if( dtt->ismapping(-1) )
		{
			yaml_event_t e;
			yaml_mapping_start_event_initialize( &e, ys(anchor), ys(tag), tag?0:1, mapping_style(style) );
			yaml_emitter_emit( em, &e );

			pcstr k = dtt->enumkey(-1);
			while( k )
			{
				dtt->push( -1, k );
				emit_key( em, k );
				emit_element( em, dtt, mid );
				dtt->pop( 1 );
				k = dtt->enumkey(-1,k);
			}

			yaml_mapping_end_event_initialize( &e );
			yaml_emitter_emit( em, &e );
		}
	}


	void refcount_element( IDatatree* dtt, uint mid, int& aid )
	{
		if( get_iprop(dtt,-1,MAGICGID_PNAME) == mid )
		{
			int refc = get_iprop(dtt,-1,REFCOUNT_PNAME) + 1;
			set_iprop( dtt, -1, REFCOUNT_PNAME, refc );

			if( refc==2 )
			{
				char anchor[32];
				SU_ASSERT( aid < 999999 );
				util::Strformat( anchor, "id%06d", aid++ );
				set_sprop( dtt, -1, ANCHOR_PNAME, anchor );
			}
		}
		else
		{
			set_iprop( dtt, -1, MAGICGID_PNAME, mid );
			set_iprop( dtt, -1, REFCOUNT_PNAME, 1 );
			set_iprop( dtt, -1, EMITCOUNT_PNAME, 0 );

			if( dtt->issequence(-1) )
			{
				int n = dtt->size(-1);
				for( int i=0 ; i<n ; i++ )
				{
					dtt->push( -1, i );
					refcount_element( dtt, mid, aid );
					dtt->pop( 1 );
				}
			}
			else if( dtt->ismapping(-1) )
			{
				pcstr k = dtt->enumkey(-1);
				while( k )
				{
					dtt->push( -1, k );
					refcount_element( dtt, mid, aid );
					dtt->pop( 1 );
					k = dtt->enumkey(-1,k);
				}
			}
		}
	}


	void emit_document( yaml_emitter_t* em, IDatatree* dtt, int incount, bool inref )
	{
		yaml_event_t e;

		for( ; incount>0 ; incount-- )
		{
			yaml_document_start_event_initialize( &e, NULL, NULL, NULL, 1 );
			if( yaml_emitter_emit(em,&e) )
			{
				uint32 mid = get_magicid();
				int    aid = 1;

				if( inref )
					refcount_element( dtt, mid, aid );

				emit_element( em, dtt, mid );
				dtt->pop( 1 );

				yaml_document_end_event_initialize( &e, 1 );
				yaml_emitter_emit( em, &e );
			}
		}
	}


	void emit_input( yaml_emitter_t* em, IDatatree* dtt, int incount, bool inref )
	{
		yaml_emitter_set_encoding( em, YAML_UTF8_ENCODING );
		yaml_emitter_set_indent( em, 4 );
	//	yaml_emitter_set_canonical( &emitter, 1 );
	//	yaml_emitter_set_unicode( &emitter, 1 );

		yaml_event_t e;
		yaml_stream_start_event_initialize( &e, YAML_ANY_ENCODING );
		if( yaml_emitter_emit(em,&e) )
		{
			emit_document( em, dtt, incount, inref );

			yaml_stream_end_event_initialize( &e );
			yaml_emitter_emit( em, &e );
		}
	}



	int emit_handler ( void* data, unsigned char* buffer, size_t sz )
	{
		IFileWriter* fw = (IFileWriter*) data;
		int r = fw->Write( buffer, sz );
		if( r==0 )	return 0;
		else		return 1;
	}








	struct SIDL_STUB_CLASSNAME
	{

		Pool*	pool;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;

			g_fatherPool = globals::authpool ? globals::authpool : pool;

			return true;
		}


		void Shut ( )
		{
			//
			g_fatherPool = globals::authpool;
		}


		int Load ( IDatatree* indtt, pcstr infromfilename )
		{
			SU_REQUIRE_OR_RETURN_V( indtt, -1 );

			IFileView* fv = globals::fs->OpenView( infromfilename, pool );
			SU_REQUIRE_OR_RETURN_V( fv, -1 );

			uint64 mbsize = fv->GetSize();
			void*  maddr = fv->Map();

			int count = 0;

			if( maddr )
				count = Parse( indtt, (byte*)maddr, mbsize );

			fv->Release();
			return count;
		}


		int Parse ( IDatatree* indtt, byte* inaddr, uint insize )
		{
			SU_REQUIRE_OR_RETURN_V( indtt, -1 );
			SU_REQUIRE_OR_RETURN_V( inaddr, -1 );
			SU_REQUIRE_OR_RETURN_V( insize, -1 );
			SU_REQUIRE_OR_RETURN_V( g_fatherPool, -1 );

			yaml_parser_t parser;

			SU_REQUIRE_OR_0( yaml_parser_initialize(&parser) );

			yaml_parser_set_input_string( &parser, inaddr, insize );

			int count = parse_input( &parser, indtt );
			bool success = (parser.error == YAML_NO_ERROR);

			yaml_parser_delete( &parser );
			return success ? count : -1;
		}


		bool Dump ( IDatatree* indtt, pcstr intofilename, int incount, bool inref )
		{
			SU_REQUIRE_OR_FALSE( indtt );
			SU_REQUIRE_OR_FALSE( indtt->gettop()+1 >= incount );
			SU_REQUIRE_OR_FALSE( globals::obspool );

			Pool* sp = globals::obspool->CreateSub();
			SU_REQUIRE_OR_FALSE( sp );
			
			IFileWriter* fw = globals::fs->OpenWriter( intofilename, sp, true, false );
			SU_REQUIRE_OR_FALSE( fw );

			yaml_emitter_t emitter;
			bool success = ( yaml_emitter_initialize(&emitter) != 0 );
			if( success )
			{
				yaml_emitter_set_output( &emitter, emit_handler, (void*)fw );
				
				emit_input( &emitter, indtt, incount, inref );
				success = (emitter.error == YAML_NO_ERROR);
				
				if( !success )
				{
					char* problem = (char*) emitter.problem;
				}
				
				yaml_emitter_flush( &emitter );
				yaml_emitter_delete( &emitter );
			}
			
			Pool::Destroy( sp );
			return success;
		}


		bool Present ( IDatatree* indtt, byte* inaddr, uint incapacity, uint& outsize, int incount, bool inref )
		{
			outsize = 0;

			SU_REQUIRE_OR_FALSE( indtt );
			SU_REQUIRE_OR_FALSE( indtt->gettop()+1 >= incount );
			SU_REQUIRE_OR_FALSE( inaddr );
			SU_REQUIRE_OR_FALSE( incapacity );
			SU_REQUIRE_OR_FALSE( g_fatherPool );

			yaml_emitter_t emitter;
			size_t emit_written = 0;
			SU_REQUIRE_OR_FALSE( yaml_emitter_initialize(&emitter) );
			yaml_emitter_set_output_string( &emitter, inaddr, (size_t)incapacity, &emit_written );

			emit_input( &emitter, indtt, incount, inref );
			bool success = (emitter.error == YAML_NO_ERROR);

			if( success )
			{
				outsize = (uint) emit_written;
			}
			else
			{
				char* problem = (char*) emitter.problem;
			}

			yaml_emitter_flush( &emitter );
			yaml_emitter_delete( &emitter );
			return success;
		}

	};


}




