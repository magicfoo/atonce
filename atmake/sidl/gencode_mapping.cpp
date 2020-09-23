


namespace
{


	template <typename T>
	void writecontent_var ( T& attr, IDatatree* dtt, map::context& ctxt, pcstr name )
	{
		SU_ASSERT( dtt );
		SU_REQUIRE_OR_RETURN( ctxt.errcode==0 );

		// optional member ?
		if( !attr.p )
			return;
		
		// ro ?
		if( attr.ro )
			return;

		if( !dtt->ismapping(-1) )
		{
			Strcopy( ctxt.varname, name );
			Strcopy( ctxt.errmsg, "A map is expected !" );
			ctxt.errcode = map::context::EC_MAP_EXPECTED;
			return;
		}

		dtt->push( -1, name );

		if( dtt->isinteger(-1) )
		{
			int v = dtt->asinteger(-1);
			for( int i=0 ; i<attr.n ; i++ )
				attr.set( v, i );
		}
		else if( dtt->isnumber(-1) )
		{
			float v = dtt->asnumber(-1);
			for( int i=0 ; i<attr.n ; i++ )
				attr.set( v, i );
		}
		else if( dtt->isboolean(-1) )
		{
			int v = dtt->asboolean(-1) ? 1 : 0;
			for( int i=0 ; i<attr.n ; i++ )
				attr.set( v, i );
		}
		else if( dtt->isstring(-1) )
		{
			pcstr p = dtt->asstring(-1);
			int   n = Strlen(p);
			if( attr.w != 8 )
			{
				Strcopy( ctxt.varname, name );
				Strcopy( ctxt.errmsg, "String requires 8-bits width attribute !" );
				ctxt.errcode = map::context::EC_STRING_8BITS;
			}
			else if( n > attr.n )
			{
				Strcopy( ctxt.varname, name );
				Strcopy( ctxt.errmsg, "String overflows the attribute reserved area !" );
				ctxt.errcode = map::context::EC_STRING_OVERFLOW;
			}
			else
			{
				ub_copy( attr.p, attr.s, (byte*)p, 0, n*8 );
				ub_zero( attr.p, attr.s+n*8, (attr.n-n)*8 );	// fill with 0!
			}
		}
		else if( dtt->isblob(-1) )
		{
			byte* p; int n;
			p = dtt->asblob(-1,&n);
			if( n*8 > attr.n*attr.w )
			{
				Strcopy( ctxt.varname, name );
				Strcopy( ctxt.errmsg, "Blob overflows the attribute area !" );
				ctxt.errcode = map::context::EC_BLOB_OVERFLOW;
			}
			else
			{
				attr.blit( p, n*8 );
			}
		}
		else if( dtt->isscalar(-1) )
		{
			int v = dtt->tointeger(-1);
			attr.set( v ); 
		}
		else if( dtt->issequence(-1) )
		{
			int n = dtt->size(-1);
			if( n > attr.n )
			{
				Strcopy( ctxt.varname, name );
				Strcopy( ctxt.errmsg, "Sequence has to many elements !" );
				ctxt.errcode = map::context::EC_SEQ_OVERFLOW;
			}
			else
			{
				for( int i=0 ; i<attr.n ; i++ )
				{
					dtt->push(-1,i);
					if( dtt->isnumber(-1) )
						attr.set( dtt->tonumber(-1), i );
					else if( dtt->isinteger(-1) )
						attr.set( dtt->tointeger(-1), i );
					else
						attr.set( 0, i );
					dtt->pop(1);
				}
			}
		}
		else
		{
			Strcopy( ctxt.varname, name );
			Strcopy( ctxt.errmsg, "Unexpected type of dtt node !" );
			ctxt.errcode = map::context::EC_INVALID_TYPE;
		}

		dtt->pop(1);
	}


	StringId push_tag ( IDatatree* dtt, int ith, map::context& ctxt )
	{
		SU_ASSERT( dtt );
		SU_ASSERT( dtt->issequence(-1) );
		SU_REQUIRE_OR_NULL( ctxt.errcode==0 );

		dtt->push(-1,ith);
		if( !dtt->ismapping(-1) )
		{
			Strcopy( ctxt.errmsg, "A mapping table is expected as a tag entry !" );
			ctxt.errcode = map::context::EC_MAP_EXPECTED;
			dtt->pop(1);
			return sid::null;
		}

		dtt->push(-1,"tagid");
		if( !dtt->isstring(-1) )
		{
			Strcopy( ctxt.errmsg, "A tag must be identified using the key 'tagid' !" );
			ctxt.errcode = map::context::EC_TAGID_MISSING;
			dtt->pop(2);
			return sid::null;
		}

		pcstr tagid = dtt->asstring(-1);
		dtt->pop(1);

		return sid::Find(tagid);
	}


	uint get_tag_req ( IDatatree* dtt, pcstr inchain, map::context& ctxt )
	{
		SU_ASSERT( dtt );
		SU_ASSERT( dtt->ismapping(-1) );
		SU_REQUIRE_OR_NULL( ctxt.errcode==0 );

		dtt->pushchain( -1, inchain );

		if( !dtt->isscalar(-1) || dtt->isblob(-1) )
		{
			Strformat( ctxt.errmsg, "The member '%s' is required !", inchain );
			ctxt.errcode = map::context::EC_MEMBER_MISSING;
			dtt->pop(1);
			return 0;
		}
		else
		{
			uint v = dtt->tointeger(-1);
			dtt->pop(1);
			return v;
		}
	}


}






