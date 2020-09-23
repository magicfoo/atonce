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



#define XSTACK_MAX_TOP		64
#define DEF_SEQUENCE_CAP	16


using namespace su;




namespace
{

	struct dtt_string
	{
		uint32			hash;
		int				len;
		inline byte*	data() { return (byte*)(this+1); }
	};

	uint32 dtt_string_hash ( pcstr incontent )
	{
		SU_REQUIRE_OR_ZERO( incontent && incontent[0] );
		return util::hmac_crc32_w( incontent, false );
	}

	dtt_string* dtt_string_alloc ( Pool* inp, pcstr incontent )
	{
		SU_REQUIRE_OR_NULL( inp );
		uint l = util::Strlen( incontent );
		SU_REQUIRE_OR_NULL( l );
		dtt_string* s = inp->New<dtt_string>( l+1 );
		SU_REQUIRE_OR_NULL( s );
		pstr ss = (pstr) s->data();
		util::Strcopy( ss, incontent );
		ss[l] = 0;
		s->hash = dtt_string_hash( ss );
		s->len = l;
		return s;
	}

	dtt_string* dtt_string_alloc ( Pool* inp, pcstr instart, pcstr inend )
	{
		SU_REQUIRE_OR_NULL( inp );
		SU_REQUIRE_OR_NULL( inend > instart );
		uint l = inend - instart;
		dtt_string* s = inp->New<dtt_string>( l+1 );
		SU_REQUIRE_OR_NULL( s );
		pstr ss = (pstr) s->data();
		util::Strcopy( ss, instart, l );
		ss[l] = 0;
		s->hash = dtt_string_hash( ss );
		s->len = l;
		return s;
	}

	void dtt_string_free ( Pool* inp, dtt_string* s )
	{
		SU_REQUIRE_OR_RETURN( inp );
		if( s )		inp->Free( s );
	}




	struct dtt_blob
	{
		uint32			hash;
		int				bsize;
		inline byte*	data() { return (byte*)(this+1); }
	};

	dtt_blob* dtt_blob_alloc ( Pool* inp, byte* inbaddr, uint inbsize )
	{
		SU_REQUIRE_OR_NULL( inp );
		SU_REQUIRE_OR_NULL( inbaddr );
		SU_REQUIRE_OR_NULL( inbsize );
		dtt_blob* u = inp->New<dtt_blob>( inbsize );
		SU_REQUIRE_OR_NULL( u );
		util::Memcopy( u->data(), inbaddr, inbsize );
		u->hash = util::hmac_crc32_w( inbaddr, inbsize );
		u->bsize = inbsize;
		return u;
	}

	void dtt_blob_free ( Pool* inp, dtt_blob* u )
	{
		SU_REQUIRE_OR_RETURN( inp );
		if( u )		inp->Free( u );
	}





	struct dtt_node
	{
		IDatatree::NodeType		type;
		int						refcount;
		dtt_node*				props;

		void init ( IDatatree::NodeType intype )
		{
			type = intype;
			refcount = 1;
			props = NULL;
		}

		virtual void release ( Pool* inp )
		{
			SU_ASSERT( inp );
			if( refcount==1 )
			{
				if( props )
				{
					props->release(inp);
					props=NULL;
				}
				inp->Free( this );
			}
			else
			{
				refcount--;
			}
		}
	};

	static void dtt_node_release ( Pool* inp, dtt_node* innode )
	{
		if( innode )	innode->release( inp );
	}

	static void dtt_node_addref ( dtt_node* innode )
	{
		if( innode )	innode->refcount++;
	}



	union dtt_content
	{
		bool			b;
		int				i;
		float			f;
		dtt_string*		s;
		dtt_blob*		u;
	};


	struct dtt_scalar : public dtt_node
	{
		IDatatree::ScalarType	stype;
		dtt_content				content;

		static const IDatatree::NodeType TYPE = IDatatree::NT_Scalar;


		static dtt_scalar* alloc	(	Pool*					inp,
										IDatatree::ScalarType	instype,
										dtt_content				incontent	)
		{
			SU_REQUIRE_OR_NULL( inp );
			SU_REQUIRE_OR_NULL( instype > IDatatree::ST_Undef );
			SU_REQUIRE_OR_NULL( instype <= IDatatree::ST_Blob );

			dtt_scalar* n = inp->New<dtt_scalar>();
			SU_REQUIRE_OR_NULL( n );

			n->init( IDatatree::NT_Scalar );
			n->stype	= instype;
			n->content	= incontent;
			return n;
		}

		void release ( Pool* inp )
		{
			SU_ASSERT( inp );
			if( refcount == 1 )
			{
				if( stype == IDatatree::ST_Blob )
					dtt_blob_free( inp, content.u );
				if( stype == IDatatree::ST_String )
					dtt_string_free( inp, content.s );
			}
			dtt_node::release( inp );
		}
	};



	struct dtt_table
	{
		int					capacity;
		int					count;
		inline dtt_node**	items() { return (dtt_node**)(this+1); }
		inline dtt_node*	get( int i ) { return items()[i]; }
		inline void			set( int i, dtt_node* n ) { items()[i] = n; }
	};


	struct dtt_sequence : public dtt_node
	{
		dtt_table*	tab;

		static const IDatatree::NodeType TYPE = IDatatree::NT_Sequence;

		static dtt_sequence* alloc	(	Pool*	inp,
										int		incapacity	)
		{
			SU_REQUIRE_OR_NULL( inp );
			SU_REQUIRE_OR_NULL( incapacity >= 0 );

			dtt_sequence* n = inp->New<dtt_sequence>();
			SU_REQUIRE_OR_NULL( n );

			n->init( IDatatree::NT_Sequence );
			n->tab		= NULL;

			n->reserve( inp, Max(incapacity,DEF_SEQUENCE_CAP) );
			return n;
		}

		void reserve ( Pool* inp, int incapacity )
		{
			SU_REQUIRE_OR_RETURN( incapacity );
			SU_REQUIRE_OR_RETURN( !tab || tab->capacity<incapacity );
			dtt_table* ot = tab;
			dtt_table* nt = inp->New<dtt_table>( incapacity*sizeof(dtt_node*) );
			SU_ASSERTC( nt, "Allocated heap limit reached !" );
			if( nt )
			{
				nt->capacity = incapacity;
				nt->count = 0;
				if( ot )
				{
					nt->count = Min( incapacity, ot->count );
					for( int i=0 ; i<nt->count ; i++ )
						nt->set( i, ot->get(i) );
				}
			}
			inp->Free( ot );
			tab = nt;
		}

		void resize ( Pool* inp, int insize )
		{
			SU_ASSERT( insize >= 0 );
			reserve( inp, insize );
			SU_ASSERT( tab );
			SU_ASSERT( insize <= tab->capacity );

			// resize up ?
			if( insize > tab->count )
			{
				for( int i=tab->count ; i<insize ; i++ )
					tab->set( i, NULL );
			}
			// resize down ?
			else if( insize < tab->count )
			{
				for( int i=insize ; i<tab->count ; i++ )
				{
					dtt_node* n = tab->get(i);
					tab->set( i, NULL );
					dtt_node_release( inp, n );
				}
			}
			tab->count = insize;
		}

		void release ( Pool* inp )
		{
			SU_ASSERT( inp );
			if( refcount == 1 )
			{
				resize( inp, 0 );
				inp->Free( tab );
			}
			dtt_node::release( inp );
		}
	};






	// based on a skip list data structure :
	//		http://igoro.com/archive/skip-lists-are-fascinating/

	struct dtt_kcell
	{
		char			h;			// height in [1,32]
		dtt_kcell**		next;		// next[h+1]
		dtt_node*		val;
		uint32			hash;
		pcstr			key;
	};

	dtt_kcell* dtt_kcell_alloc ( Pool* inp, char inh, pcstr inkey )
	{
		SU_REQUIRE_OR_NULL( inp );
		SU_REQUIRE_OR_NULL( inh >= 1 );
		SU_REQUIRE_OR_NULL( inh <= 32 );

		uint l = util::Strlen( inkey );
		SU_REQUIRE_OR_NULL( l );

		dtt_kcell* c = inp->New<dtt_kcell>( sizeof(dtt_kcell*)*inh + (l+1) );
		SU_REQUIRE_OR_NULL( c );
		c->next		= (dtt_kcell**)(c+1);
		c->h		= inh;
		c->val		= NULL;
		c->hash		= dtt_string_hash( inkey );
		c->key		= (pcstr)(c->next+inh);
		util::Zero( c->next, sizeof(dtt_kcell*)*inh );
		util::Strcopy( (pstr)c->key, inkey );
		return c;
	}


	struct dtt_klist
	{
		int				count;
		dtt_kcell*		head;
	};

	dtt_klist* dtt_klist_alloc ( Pool* inp )
	{
		dtt_kcell* c = dtt_kcell_alloc( inp, 32, "h" );
		SU_REQUIRE_OR_NULL( c );

		dtt_klist* l = inp->New<dtt_klist>();
		SU_REQUIRE_OR_NULL( l );

		l->count = 0;
		l->head = c;
		return l;
	}

	dtt_kcell* dtt_klist_find ( dtt_klist* inkl, uint32 inhash )
	{
		SU_REQUIRE_OR_NULL( inkl );
		SU_REQUIRE_OR_NULL( inhash );

		dtt_kcell	*nxt,
					*cur = inkl->head;

		for( int i=31 ; i>=0 ; i-- )
		{
			for( ; (nxt=cur->next[i]) ; (cur=nxt) )
			{
				if( nxt->hash > inhash )	break;
				if( nxt->hash == inhash )	return nxt;
			}
		}
		return NULL;
	}

	dtt_kcell* dtt_klist_insert ( Pool* inp, dtt_klist* inkl, uint32 inhash, pcstr inkey )
	{
		SU_REQUIRE_OR_NULL( inp );
		SU_REQUIRE_OR_NULL( inkl );
		SU_REQUIRE_OR_NULL( inhash );
		SU_ASSERT( dtt_klist_find(inkl,inhash) == NULL );

		int h = (inhash&31)+1;
		dtt_kcell* ins = dtt_kcell_alloc( inp, h, inkey );
		inkl->count++;

		dtt_kcell	*nxt,
					*cur = inkl->head;

		for( int i=31 ; i>=0 ; i-- )
		{
			for( ; (nxt=cur->next[i]) ; (cur=nxt) )
			{
				SU_ASSERT( nxt->hash != inhash );
				if( nxt->hash > inhash )	break;
			}
			if( i < ins->h )
			{
				ins->next[i] = cur->next[i];
				cur->next[i] = ins;
			}
		}

		return ins;
	}

	dtt_kcell* dtt_klist_remove( Pool* inp, dtt_klist* inkl, uint32 inhash )
	{
		SU_REQUIRE_OR_FALSE( inp );
		SU_REQUIRE_OR_FALSE( inkl );
		SU_REQUIRE_OR_FALSE( inhash );

		dtt_kcell	*nxt,
					*cur   = inkl->head,
					*found = NULL;

		for( int i=31 ; i>=0 ; i-- )
		{
			for( ; (nxt=cur->next[i]) ; (cur=nxt) )
			{
				if( nxt->hash == inhash )
				{
					SU_ASSERT( !found || found==nxt );
					found = nxt;
					cur->next[i] = nxt->next[i];
					break;
				}
				if( nxt->hash > inhash )
					break;
			}
		}

		if( found )
			inkl->count--;

		return found;
	}



	struct dtt_mapping : public dtt_node
	{
		dtt_klist*	klist;

		static const IDatatree::NodeType TYPE = IDatatree::NT_Mapping;

		static dtt_mapping* alloc ( Pool* inp )
		{
			SU_REQUIRE_OR_NULL( inp );

			dtt_klist* l = dtt_klist_alloc( inp );
			SU_REQUIRE_OR_NULL( l );

			dtt_mapping* n = inp->New<dtt_mapping>();
			SU_REQUIRE_OR_NULL( n );

			n->init( IDatatree::NT_Mapping );
			n->klist	= l;
			return n;
		}

		void release ( Pool* inp )
		{
			SU_ASSERT( inp );
			if( refcount == 1 )
			{
				dtt_kcell *c=klist->head, *n;
				while( c )
				{
					n = c->next[0];
					inp->Free( c );
					c = n;
				}
				inp->Free( klist );
			}
			dtt_node::release( inp );
		}
	};




	struct dtt_stack
	{
		dtt_node*	slot[ XSTACK_MAX_TOP ];
		int			top;

		static dtt_stack* alloc ( Pool*	inp	)
		{
			SU_REQUIRE_OR_NULL( inp );

			dtt_stack* s = inp->New<dtt_stack>();
			SU_REQUIRE_OR_NULL( s );

			util::Zero( s->slot );
			s->top = -1;
			return s;
		}

		int size ( )
		{
			SU_ASSERT( top >= -1 );
			return top+1;
		}

		void resize ( Pool* inp, int inn )
		{
			SU_REQUIRE_OR_RETURN( inp );
			inn = Clamp( inn, 0, XSTACK_MAX_TOP );
			int s = size();
			int d = inn - s;
			if( d > 0 )
			{
				while( d-- )
					slot[++top] = NULL;
			}
			else if( d < 0 )
			{
				while( d++ )
				{
					dtt_node* n = slot[top--];
					dtt_node_release( inp, n );
				}
			}
			SU_ASSERT( size() == inn );
		}

		int abs_idx ( int inidx )
		{
			return (inidx<0) ? (inidx+top+1) : (inidx);
		}

		bool check_idx ( int inidx )
		{
			int aidx = abs_idx( inidx );
			return (aidx>=0 && aidx<=top);
		}

		bool check_insert ( int inn=0 )
		{
			SU_ASSERT( inn >= 0 );
			return (top+inn) < XSTACK_MAX_TOP;
		}

		dtt_node* get_slot ( int inidx )
		{
			int idx = abs_idx( inidx );
			SU_ASSERT( idx >= 0 );
			dtt_node* n = slot[idx];
			SU_ASSERT_IF( n, n->refcount );
			return n;
		}

		template <typename T> T* get_typed_slot ( int inidx )
		{
			dtt_node* n = get_slot( inidx );
			SU_REQUIRE_OR_NULL( n );
			SU_REQUIRE_OR_NULL( n->type == T::TYPE );
			return (T*)n;
		}

		dtt_node* get_top ( )
		{
			SU_ASSERT( top >= 0 );
			dtt_node* n = slot[top];
			SU_ASSERT_IF( n, n->refcount );
			return n;
		}

		dtt_node* pop ( )
		{
			SU_ASSERT( top >= 0 );
			dtt_node* n = slot[top--];
			SU_ASSERT_IF( n, n->refcount );
			return n;
		}

		void push ( dtt_node* n )
		{
			SU_ASSERT( top+1 < XSTACK_MAX_TOP );
			slot[++top] = n;
		}
	};





	struct SIDL_STUB_CLASSNAME
	{
		Pool*			pool;
		dtt_stack*		stack;


		bool Init ( Pool* p, AnyList* o )
		{
			pool  = NULL;
			stack = NULL;
			return Reset( p );
		}


		void Shut ( )
		{
			destroy();
		}


		void destroy ( )
		{
			Pool::Destroy( pool );
			stack = NULL;
		}


		bool Reset ( Pool* p0 )
		{
			SU_REQUIRE_OR_FALSE( p0 || pool );

			if( p0 )
			{
				destroy();
				pool = p0->CreateSub();
				SU_REQUIRE_OR_FALSE( pool );
			}
			else
			{
				pool->Clear();
			}

			stack = dtt_stack::alloc( pool );
			SU_REQUIRE_OR_FALSE( stack );

			return true;
		}


		bool isempty ( )
		{
			SU_REQUIRE_OR_TRUE( stack );
			return (stack->top<0);
		}


		int getcount ( )
		{
			SU_REQUIRE_OR_0( stack );
			return stack->top+1;
		}


		void setcount ( int inn )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( inn>=0 );
			stack->resize( pool, inn );
		}


		int baseindex ( int inidx )
		{
			SU_REQUIRE_OR_ZERO( stack );
			return (inidx<0) ? (stack->top+1+inidx) : (inidx);
		}


		int topindex ( int inidx )
		{
			SU_REQUIRE_OR_ZERO( stack );
			return (inidx<0) ? (inidx) : -(stack->top-inidx)-1;
		}


		int gettop ( )
		{
			SU_REQUIRE_OR_ZERO( stack );
			return stack->top;
		}


		void settop ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( inidx != -1 );
			int bidx = baseindex( inidx );
			stack->resize( pool, bidx+1 );
		}


		void pop ( int inn )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( inn>0 );
			stack->resize( pool, stack->size()-inn );
		}


		void push ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_node* n = stack->get_slot(inidx);
			stack->push(n);
			dtt_node_addref( n );
		}


		void remove ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			int idx = stack->abs_idx( inidx );
			dtt_node* n = stack->get_slot(idx);
			dtt_node_release( pool, n );
			for( int i=idx ; i<stack->top ; i++ )
				stack->slot[i] = stack->slot[i+1];
			stack->top--;
		}


		void insert ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			SU_ASSERT( stack->check_idx(inidx) );
			int idx = stack->abs_idx( inidx );
			if( inidx < stack->top ) {
				dtt_node* n = stack->get_top();
				for( int i=stack->top ; i>idx ; i-- )
					stack->slot[i] = stack->slot[i-1];
				stack->slot[idx] = n;
			}
		}


		void replace ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			int idx = stack->abs_idx( inidx );
			if( idx == stack->top ) {
				pop( 1 );
			} else {
				dtt_node* n = stack->slot[idx];
				dtt_node_release( pool, n );
				stack->slot[idx] = stack->pop();
			}
		}


		bool equal ( int inidx1, int inidx2 )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx1) );
			SU_ASSERT( stack->check_idx(inidx2) );
			dtt_node* n1 = stack->get_slot(inidx1);
			dtt_node* n2 = stack->get_slot(inidx2);
			if( n1 == n2 )				return true;
			if( !n1 || !n2 )			return false;
			if( n1->type != n2->type )	return false;
			if( n1->type == IDatatree::NT_Scalar )
			{
				dtt_scalar* s1 = (dtt_scalar*)n1;
				dtt_scalar* s2 = (dtt_scalar*)n2;
				if( s1->stype != s2->stype )	return false;
				switch( s1->stype )
				{
					case IDatatree::ST_Boolean :
					{
						return (s1->content.b == s2->content.b);
					}
					case IDatatree::ST_Integer :
					{
						return (s1->content.i == s2->content.i);
					}
					case IDatatree::ST_Number :
					{
						return (s1->content.f == s2->content.f);
					}
					case IDatatree::ST_String :
					{
						dtt_string* str1 = s1->content.s;
						dtt_string* str2 = s2->content.s;
						if( str1 == str2 )				return true;
						if( str1->hash == str2->hash )	return true;
						return false;
					}
					case IDatatree::ST_Blob :
					{
						dtt_blob* b1 = s1->content.u;
						dtt_blob* b2 = s2->content.u;
						if( b1 == b2 )				return true;
						if( b1->hash == b2->hash )	return true;
						return false;
					}
				}
			}
			return false;
		}


		bool isnil ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			int idx = stack->abs_idx( inidx );
			dtt_node* n = stack->slot[idx];
			return (n == NULL);
		}


		bool isboolean ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_FALSE( n );
			return (n->stype == IDatatree::ST_Boolean);
		}


		bool isblob ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_FALSE( n );
			return (n->stype == IDatatree::ST_Blob);
		}


		bool isstring ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_FALSE( n );
			return (n->stype == IDatatree::ST_String);
		}


		bool isinteger ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_FALSE( n );
			return (n->stype == IDatatree::ST_Integer);
		}


		bool isnumber ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_FALSE( n );
			return (n->stype == IDatatree::ST_Number);
		}


		bool isscalar ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			return (n != NULL);
		}
	

		bool iscollection ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_sequence* c1 = stack->get_typed_slot<dtt_sequence>( inidx );
			dtt_mapping*  c2 = stack->get_typed_slot<dtt_mapping>( inidx );
			return (c1 || c2);
		}


		bool issequence ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_sequence* n = stack->get_typed_slot<dtt_sequence>( inidx );
			return (n != NULL);
		}


		bool ismapping ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_mapping* n = stack->get_typed_slot<dtt_mapping>( inidx );
			return (n != NULL);
		}


		IDatatree::NodeType nodetype ( int inidx )
		{
			SU_REQUIRE_OR_RETURN_V( stack, IDatatree::NT_Nil );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_node* n = stack->get_slot(inidx);
			if( n )		return n->type;
			else		return IDatatree::NT_Nil;
		}


		IDatatree::ScalarType scalartype ( int inidx )
		{
			SU_REQUIRE_OR_RETURN_V( stack, IDatatree::ST_Undef );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			if( n )		return n->stype;
			else		return IDatatree::ST_Undef;
		}


		bool asboolean ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_FALSE( n );
			SU_REQUIRE_OR_FALSE( n->stype == IDatatree::ST_Boolean );
			return n->content.b;
		}


		pcstr asstring ( int inidx )
		{
			SU_REQUIRE_OR_NULL( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_NULL( n );
			SU_REQUIRE_OR_NULL( n->stype == IDatatree::ST_String );
			if( n->content.s )		return (pcstr) n->content.s->data();
			else					return NULL;
		}


		int asinteger ( int inidx )
		{
			SU_REQUIRE_OR_ZERO( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_ZERO( n );
			SU_REQUIRE_OR_ZERO( n->stype == IDatatree::ST_Integer );
			return n->content.i;
		}


		float asnumber ( int inidx )
		{
			SU_REQUIRE_OR_ZERO( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_ZERO( n );
			SU_REQUIRE_OR_ZERO( n->stype == IDatatree::ST_Number );
			return n->content.f;
		}


		byte* asblob ( int inidx, int* outBsize )
		{
			SU_REQUIRE_OR_NULL( stack );
			SU_ASSERT( stack->check_idx(inidx) );
			dtt_scalar* n = stack->get_typed_slot<dtt_scalar>( inidx );
			SU_REQUIRE_OR_NULL( n );
			SU_REQUIRE_OR_NULL( n->stype == IDatatree::ST_Blob );
			dtt_blob* u = n->content.u;
			if( outBsize )	*outBsize = (int)u->bsize;
			return u->data();
		}


		bool toboolean ( int inidx )
		{
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				return asboolean(inidx);
			}
			else if( st == IDatatree::ST_Integer )
			{
				return asinteger(inidx) != 0;
			}
			else if( st == IDatatree::ST_Number )
			{
				return asnumber(inidx) != 0.f;
			}
			else if( st == IDatatree::ST_String )
			{
				pcstr s = asstring(inidx);
				if( s && *s )
				{
					if( util::Strequals(s,"true",false) )		return true;
					if( util::Strequals(s,"yes",false) )		return true;
					if( util::Strequals(s,"oui",false) )		return true;
					if( util::Strequals(s,"vrai",false) )		return true;
					if( util::Strequals(s,"1",false) )			return true;
				}
			}
			return false;
		}


		pcstr tostring ( int inidx )
		{
			static char shared[16];
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				bool b = asboolean(inidx);
				if( b )		return "true";
				else		return "false";
			}
			else if( st == IDatatree::ST_Integer )
			{
				int i = asinteger(inidx);
				int len = util::Strformat( shared, (long)i, 10 );
				SU_ASSERT( len<sizeof(shared) );
				return shared;
			}
			else if( st == IDatatree::ST_Number )
			{
				float n = asnumber(inidx);
				int len = util::Strformat( shared, n );
				SU_ASSERT( len<sizeof(shared) );
				return shared;
			}
			else if( st == IDatatree::ST_String )
			{
				return asstring(inidx);
			}
			return NULL;
		}


		int tointeger ( int inidx )
		{
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				bool b = asboolean(inidx);
				if( b )		return 1;
				else		return 0;
			}
			else if( st == IDatatree::ST_Integer )
			{
				int i = asinteger(inidx);
				return i;
			}
			else if( st == IDatatree::ST_Number )
			{
				float n = asnumber(inidx);
				return int(n);
			}
			else if( st == IDatatree::ST_String )
			{
				pcstr s = asstring(inidx);
				if( s && *s )
					return util::Strtoi10(s);
			}
			return 0;
		}


		float tonumber ( int inidx )
		{
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				bool b = asboolean(inidx);
				if( b )		return 1.f;
				else		return 0.f;
			}
			else if( st == IDatatree::ST_Integer )
			{
				int i = asinteger(inidx);
				return float(i);
			}
			else if( st == IDatatree::ST_Number )
			{
				float n = asnumber(inidx);
				return n;
			}
			else if( st == IDatatree::ST_String )
			{
				pcstr s = asstring(inidx);
				if( s && *s )
					return util::Strtofloat(s,NULL);
			}
			return 0.f;
		}

			
		void clear ( int inidx )
		{
			resize( inidx, 0 );
		}


		int size ( int inidx )
		{
			SU_REQUIRE_OR_0( stack );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_node* n = stack->get_slot(inidx);
			SU_REQUIRE_OR_0( n );
			if( n->type == IDatatree::NT_Sequence )
			{
				dtt_sequence* s = (dtt_sequence*) n;
				if( s->tab )	return s->tab->count;
				else			return 0;
			}
			else if( n->type == IDatatree::NT_Mapping )
			{
				dtt_mapping* m = (dtt_mapping*) n;
				return m->klist->count;
			}
			else
			{
				return 0;
			}
		}


		void resize ( int inidx, int inlen )
		{
			SU_ASSERT( inlen >= 0 );
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_node* n = stack->get_slot(inidx);

			if( n->type == IDatatree::NT_Sequence )
			{
				dtt_sequence* s = (dtt_sequence*) n;
				s->resize( pool, inlen );
			}
		}


		void push ( int inidx, int inith )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_sequence* n = stack->get_typed_slot<dtt_sequence>( inidx );
			SU_ASSERT( n );
			SU_ASSERT( n->tab );
			SU_ASSERT( inith >= 0 );
			SU_ASSERT( inith < n->tab->count );

			dtt_node* ith = n->tab->get(inith);
			dtt_node_addref( ith );

			stack->push( ith );
		}


		void store ( int inidx, int inith )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_sequence* n = stack->get_typed_slot<dtt_sequence>( inidx );
			SU_ASSERT( n );
			SU_ASSERT( n->tab );
			SU_ASSERT( inith >= 0 );

			if( inith >= n->tab->count )
				n->resize( pool, inith+1 );
			SU_ASSERT( inith < n->tab->count );

			dtt_node* t = stack->pop();
			dtt_node_addref( t );

			dtt_node* ith = n->tab->get(inith);
			dtt_node_release( pool, ith );
			n->tab->set(inith,t);

			dtt_node_release( pool, t );
		}


		void append ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_sequence* n = stack->get_typed_slot<dtt_sequence>( inidx );

			dtt_node* t = stack->pop();

			if( n )
			{
				int sz = n->tab ? n->tab->count : 0;
				n->resize( pool, sz+1 );
				dtt_node_addref( t );
				n->tab->set(sz,t);
			}

			dtt_node_release( pool, t );
		}


		void push ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_node* k = NULL;
			dtt_mapping* n = stack->get_typed_slot<dtt_mapping>( inidx );
			if( n )
			{
				uint32 h = dtt_string_hash( inkey );
				if( h )
				{
					dtt_kcell* c = dtt_klist_find( n->klist, h );
					if( c )
					{
						k = c->val;
					}
				}
			}
			dtt_node_addref( k );
			stack->push( k );
		}


		struct KeyPath
		{
			pcstr cur;
			char  buff[64];
			int   depth;

			KeyPath ( pcstr inpath )
			{
				cur = inpath;
				depth = 1;
			}

			pcstr extract ( )
			{
				SU_REQUIRE_OR_NULL( cur && *cur );
				char *b    = buff;
				char *bend = buff + sizeof(buff);
				while( *cur && *cur!='.' && b<bend )
					*b++ = *cur++;
				SU_REQUIRE_OR_NULL( b<bend );
				*b = 0;
				if( *cur=='.' )	cur++;
				depth++;
				return buff;
			}
		};


		void pushchain ( int inidx, pcstr inkeypath )
		{
			SU_REQUIRE_OR_RETURN( stack );

			int idx = stack->abs_idx(inidx);
			SU_REQUIRE_OR_RETURN( ismapping(idx) || issequence(idx) );

			push( idx );

			bool failed = true;
			KeyPath kp( inkeypath );
			for( ;; )
			{
				pcstr co = kp.extract();
				if( !co )	break;

				if( ismapping(-1) )
				{
					push(-1,co);
					remove(-2);
					failed = false;
					continue;
				}

				if( issequence(-1) )
				{
					if( util::Strisdigit(co) )
					{
						int ith = util::Strtou10(co);
						if( ith<size(-1) )
						{
							push(-1,ith);
							remove(-2);
							failed = false;
							continue;
						}
					}
				}

				// not found !
				failed = true;
				break;
			}

			if( failed )
			{
				pop( 1 );
				pushnil();
			}
		}


		void store ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(-1) );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_mapping* n = stack->get_typed_slot<dtt_mapping>( inidx );
			SU_ASSERT( n );

			uint32 h = dtt_string_hash( inkey );
			SU_ASSERT( h );

			dtt_node* v = stack->pop();

			if( v )	// nil ?
			{
				// setup klist
				dtt_kcell* c = dtt_klist_find( n->klist, h );
				if( !c ) {
					// new entry
					c = dtt_klist_insert( pool, n->klist, h, inkey );
					SU_REQUIRE_OR_RETURN( c );
				}

				dtt_node_release( pool, c->val );
				c->val = v;
			}
			else
			{
				// unmap
				dtt_kcell* c = dtt_klist_remove( pool, n->klist, h );
				if( c )
				{
					dtt_node_release( pool, c->val );
					pool->Free( c );
				}
			}
		}


		bool haskey ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_FALSE( stack );
			SU_REQUIRE_OR_FALSE( stack->check_insert(1) );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_mapping* n = stack->get_typed_slot<dtt_mapping>( inidx );
			SU_REQUIRE_OR_FALSE( n );

			uint32 h = dtt_string_hash( inkey );
			SU_REQUIRE_OR_FALSE( h );

			dtt_kcell* c = dtt_klist_find( n->klist, h );
			SU_REQUIRE_OR_FALSE( c );

			return true;
		}


		void unkey ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_mapping* n = stack->get_typed_slot<dtt_mapping>( inidx );
			SU_ASSERT( n );

			uint32 h = dtt_string_hash( inkey );
			SU_ASSERT( h );

			dtt_kcell* c = dtt_klist_remove( pool, n->klist, h );
			SU_REQUIRE_OR_RETURN( c );

			dtt_node_release( pool, c->val );
			pool->Free( c );
		}


		pcstr enumkey ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_NULL( stack );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_mapping* n = stack->get_typed_slot<dtt_mapping>( inidx );
			SU_REQUIRE_OR_NULL( n );
			SU_REQUIRE_OR_NULL( n->klist->count );

			dtt_kcell* c;

			uint32 h = dtt_string_hash( inkey );
			if( h == 0 )	c = n->klist->head;
			else			c = dtt_klist_find( n->klist, h );

			SU_REQUIRE_OR_NULL( c );
			SU_REQUIRE_OR_NULL( c->next[0] );
			return c->next[0]->key;
		}


		void pushnil (  )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			stack->push( NULL );
		}


		void pushboolean ( bool invalue )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			dtt_content content;
			content.b = invalue;
			dtt_scalar* n = dtt_scalar::alloc( pool, IDatatree::ST_Boolean, content );
			SU_REQUIRE_OR_RETURN( n );
			stack->push( n );
		}


		void pushinteger ( int invalue )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			dtt_content content;
			content.i = invalue;
			dtt_scalar* n = dtt_scalar::alloc( pool, IDatatree::ST_Integer, content );
			SU_REQUIRE_OR_RETURN( n );
			stack->push( n );
		}


		void pushnumber ( float invalue )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			dtt_content content;
			content.f = invalue;
			dtt_scalar* n = dtt_scalar::alloc( pool, IDatatree::ST_Number, content );
			SU_REQUIRE_OR_RETURN( n );
			stack->push( n );
		}


		void pushstring ( pcstr invalue )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			if( !invalue || *invalue==0 )
			{
				stack->push( NULL );
			}
			else
			{
				dtt_content content;
				content.s = dtt_string_alloc( pool, invalue );
				SU_REQUIRE_OR_RETURN( content.s );
				dtt_scalar* n = dtt_scalar::alloc( pool, IDatatree::ST_String, content );
				SU_REQUIRE_OR_RETURN( n );
				stack->push( n );
			}
		}


		void pushstring ( pcstr instart, pcstr inend )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			if( !instart || !inend || inend<=instart )
			{
				stack->push( NULL );
			}
			else
			{
				dtt_content content;
				content.s = dtt_string_alloc( pool, instart, inend );
				SU_REQUIRE_OR_RETURN( content.s );
				dtt_scalar* n = dtt_scalar::alloc( pool, IDatatree::ST_String, content );
				SU_REQUIRE_OR_RETURN( n );
				stack->push( n );
			}
		}


		void pushblob ( byte* inaddr, int inlen )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			if( !inaddr || !inlen )
			{
				stack->push( NULL );
			}
			else
			{
				dtt_content content;
				content.u = dtt_blob_alloc( pool, inaddr, inlen );
				SU_REQUIRE_OR_RETURN( content.u );
				dtt_scalar* n = dtt_scalar::alloc( pool, IDatatree::ST_Blob, content );
				SU_REQUIRE_OR_RETURN( n );
				stack->push( n );
			}
		}


		void pushscalar ( pcstr invalue )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			pcstr v0 = invalue;
			pcstr v1 = v0 + util::Strlen(v0);

			if( util::Strisdigit(v0) )
			{
				pushinteger( util::Strtoi10(invalue) );
				return;
			}

			pstr endptr;
			float f = util::Strtofloat(v0,&endptr);
			if( endptr==v1 )
			{
				pushnumber( f );
				return;
			}

			if( util::Strequals(invalue,"false",false) )		pushboolean(false); 
			else if( util::Strequals(invalue,"no",false) )		pushboolean(false); 
			else if( util::Strequals(invalue,"non",false) )	pushboolean(false); 
			else if( util::Strequals(invalue,"faux",false) )	pushboolean(false); 
			else if( util::Strequals(invalue,"true",false) )	pushboolean(true); 
			else if( util::Strequals(invalue,"yes",false) )	pushboolean(true); 
			else if( util::Strequals(invalue,"oui",false) )	pushboolean(true); 
			else if( util::Strequals(invalue,"vrai",false) )	pushboolean(true);
			else												pushstring(invalue);
		}


		void pushsequence ( int incapacity )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			dtt_sequence* n = dtt_sequence::alloc( pool, incapacity );
			SU_REQUIRE_OR_RETURN( n );
			stack->push( n );
		}


		void pushmapping ( )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			dtt_mapping* n = dtt_mapping::alloc( pool );
			SU_REQUIRE_OR_RETURN( n );
			stack->push( n );
		}





		//
		// Properties

		void storeproperty ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_ASSERT( stack->check_idx(-1) );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_node* n = stack->get_slot( inidx );
			dtt_node* p = stack->pop();

			if( n ) {
				dtt_node_release( pool, n->props );
				n->props = p;
			} else {
				dtt_node_release( pool, p );
			}
		}


		void pushproperty ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( stack );
			SU_REQUIRE_OR_RETURN( stack->check_insert(1) );
			SU_ASSERT( stack->check_idx(inidx) );

			dtt_node* n = stack->get_slot( inidx );
			dtt_node* p = n ? n->props : NULL;
			dtt_node_addref( p );
			stack->push( p );
		}

	};


}


