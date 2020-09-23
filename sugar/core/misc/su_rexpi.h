/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
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




#ifndef _su_rexpi_h_
#define _su_rexpi_h_




namespace su {	namespace rexp
{

	template<typename iterator, typename stack>
	void NullAction::apply ( stack& s, uint position, iterator first, iterator last ) 
	{
		//
	}

	template <typename UserToken>
	template<typename iterator, typename stack>
	void TokenAction<UserToken>::apply ( stack& s, uint position, iterator first, iterator last )
	{
		if( first != last )
		{
			typename stack::value_type r;
			UserToken::token( r, first, last );
			s.push_back(r);
		}
	}

	template <typename UserReduce>
	template<typename iterator, typename stack>
	void ReduceAction<UserReduce>::apply ( stack& s, uint position, iterator first, iterator last )
	{
		if( first != last )
		{
			typename stack::value_type r;
			uint ssize = s.size();
			SU_ASSERT( position <= ssize );
			SU_ASSERT( ssize != 0 );
			uint argc = ssize-position;
			SU_ASSERT( argc > 0 );
			UserReduce::reduce( r, argc, s.addr(position) );
			s.resize( position );
			s.push_back(r);
		}
	}


	template <typename iterator> inline
	MatchResult<iterator>::MatchResult ( )
	{
		//
	}

	template <typename iterator> inline
	MatchResult<iterator>::MatchResult ( iterator inpos, bool insuccess ) : pos(inpos), success(insuccess)
	{
		//
	}

	template <typename iterator> inline
	MatchResult<iterator> MatchResult<iterator>::Failed ( iterator inpos )
	{
		return MatchResult( inpos, false );
	}

	template <typename iterator> inline
	MatchResult<iterator> MatchResult<iterator>::Successed ( iterator inpos )
	{
		return MatchResult( inpos, true );
	}



	template<int C, typename Action>
	template<typename iterator, typename stack>
	MatchResult<iterator> re_char<C,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		if( first==end || *first!=C )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			Action::apply( s, sz, first, first+1 );
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C, typename Action>
	template<typename iterator>
	MatchResult<iterator> re_char<C,Action>::match( iterator first, iterator end )
	{
		if( first==end || *first!=C )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C, typename Action>
	template<typename iterator, typename stack>
	MatchResult<iterator> re_nchar<C,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		if( first==end || *first==C )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			Action::apply( s, sz, first, first+1 );
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C, typename Action>
	template<typename iterator>
	MatchResult<iterator> re_nchar<C,Action>::match( iterator first, iterator end )
	{
		if( first==end || *first==C )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C0,int C1, typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_range<C0,C1,Action>::match( iterator first, iterator end, stack& s )
	{
		SU_COMPILE_TIME_ASSERT( C0 < C1 );
		int sz = s.size();

		if( first==end || *first<C0 || *first>C1 )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			Action::apply( s, sz, first, first+1 );
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C0,int C1, typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_range<C0,C1,Action>::match( iterator first, iterator end )
	{
		SU_COMPILE_TIME_ASSERT( C0 < C1 );

		if( first==end || *first<C0 || *first>C1 )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C0,int C1, typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_nrange<C0,C1,Action>::match( iterator first, iterator end, stack& s )
	{
		SU_COMPILE_TIME_ASSERT( C0 < C1 );
		int sz = s.size();

		if( first==end || (*first>=C0 && *first<=C1) )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			Action::apply( s, sz, first, first+1 );
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<int C0,int C1, typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_nrange<C0,C1,Action>::match( iterator first, iterator end )
	{
		SU_COMPILE_TIME_ASSERT( C0 < C1 );

		if( first==end || (*first>=C0 && *first<=C1) )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template <typename Action>
	template<typename iterator, typename stack>
	MatchResult<iterator> re_any<Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		if( first==end )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			Action::apply( s, sz, first, first+1 );
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template <typename Action>
	template<typename iterator>
	MatchResult<iterator> re_any<Action>::match( iterator first, iterator end )
	{
		if( first==end )
		{
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			return MatchResult<iterator>::Successed(first+1);
		}
	}


	template<typename re,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_action<re,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m = re::match( first, end, s );
		if( m.success )
		{
			Action::apply( s, sz, first, m.pos );
			return m;
		}

		s.resize( sz );
		return MatchResult<iterator>::Failed(first);
	}


	template<typename re,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_action<re,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m = re::match( first, end);
		if( m.success )
			return m;

		return MatchResult<iterator>::Failed(first);
	}


	template<typename re,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_cdata<re,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		iterator it = first;

		for( ;; )
		{
			if( it==end ) break;

			MatchResult<iterator> m = re::match( it, end, s );
			s.resize( sz );

			if( m.success )
			{
				Action::apply( s, sz, first, it );
				return MatchResult<iterator>::Successed(it);
			}

			++it;
		}

		s.resize( sz );
		return MatchResult<iterator>::Failed(first);
	}


	template<typename re,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_cdata<re,Action>::match( iterator first, iterator end )
	{
		iterator it = first;

		for( ;; )
		{
			if( it==end ) break;

			MatchResult<iterator> m = re::match( it, end);

			if( m.success )
				return MatchResult<iterator>::Successed(it);

			++it;
		}

		return MatchResult<iterator>::Failed(first);
	}


	template<typename re0, typename re1,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_follow<re0,re1,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m0 = re0::match( first, end, s );
		if( m0.success )
		{
			MatchResult<iterator> m1 = re1::match( m0.pos, end, s );
			if( m1.success )
			{
				Action::apply( s, sz, first, m1.pos );
				return m1;
			}
		}

		s.resize( sz );
		return MatchResult<iterator>::Failed(first);
	}


	template<typename re0, typename re1,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_follow<re0,re1,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m0 = re0::match( first, end );
		if( m0.success )
		{
			MatchResult<iterator> m1 = re1::match( m0.pos, end );
			if( m1.success )
				return m1;
		}

		return MatchResult<iterator>::Failed(first);
	}


	template<typename re0, typename re1,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_followng<re0,re1,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m = re_cdata<re1>::match( first, end, s );
		s.resize( sz );

		if( m.success )
		{
			MatchResult<iterator> m0 = re0::match( first, m.pos, s );
			if( m0.success && m0.pos==m.pos )
			{
				MatchResult<iterator> m1 = re1::match( m0.pos, end, s );
				if( m1.success )
				{
					Action::apply( s, sz, first, m1.pos );
					return m1;
				}
			}
		}

		s.resize( sz );
		return MatchResult<iterator>::Failed(first);
	}


	template<typename re0, typename re1,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_followng<re0,re1,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m = re_cdata<re1>::match( first, end );

		if( m.success )
		{
			MatchResult<iterator> m0 = re0::match( first, m.pos );
			if( m0.success && m0.pos==m.pos )
			{
				MatchResult<iterator> m1 = re1::match( m0.pos, end );
				if( m1.success )
					return m1;
			}
		}

		return MatchResult<iterator>::Failed(first);
	}


	template<typename re0, typename re1,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_or<re0,re1,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m0, m1;
		int s0, s1;
		
		m0 = re0::match( first, end, s );
		s0 = s.size();

		m1 = re1::match( first, end, s );
		s1 = s.size();

		int l0 = m0.success ? m0.pos-first : 0;
		int l1 = m1.success ? m1.pos-first : 0;

		if( l0 || l1 )
		{
			if( l0 > l1 )
			{
				s.resize( s0 );
				Action::apply( s, sz, first, m0.pos );
				return m0;
			}
			else
			{
				s.erase( s.begin()+sz, s.begin()+s0 );
				Action::apply( s, sz, first, m1.pos );
				return m1;
			}
		}

		s.resize( sz );
		return MatchResult<iterator>::Failed(first);
	}


	template<typename re0, typename re1,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_or<re0,re1,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m0, m1;

		m0 = re0::match( first, end );
		m1 = re1::match( first, end );

		int l0 = m0.success ? m0.pos-first : 0;
		int l1 = m1.success ? m1.pos-first : 0;

		if( l0 || l1 )
		{
			if( l0 > l1 )
				return m0;
			else
				return m1;
		}

		return MatchResult<iterator>::Failed(first);
	}


	template<typename re,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_star<re,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m( first, true );

		while( m.success )
			m = re::match( m.pos, end, s );

		Action::apply( s, sz, first, m.pos );
		return MatchResult<iterator>::Successed(m.pos);
	}


	template<typename re,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_star<re,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m( first, true );

		while( m.success )
			m = re::match( m.pos, end );

		return MatchResult<iterator>::Successed(m.pos);
	}


	template<typename re,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_opt<re,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m = re::match( first, end, s );

		if( m.success )
			Action::apply( s, sz, first, m.pos );

		return MatchResult<iterator>::Successed(m.pos);
	}


	template<typename re,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_opt<re,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m = re::match( first, end );

		return MatchResult<iterator>::Successed(m.pos);
	}


	template<typename re,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_token<re,Action>::match( iterator first, iterator end, stack& s )
	{
		int sz = s.size();

		MatchResult<iterator> m0 = _npN::match( first, end, s );
		MatchResult<iterator> m1 = re::match( m0.pos, end, s );

		if( m1.success )
		{
			Action::apply( s, sz, m0.pos, m1.pos );
			return m1;
		}
		else
		{
			s.resize( sz );
			return MatchResult<iterator>::Failed(first);
		}
	}


	template<typename re,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_token<re,Action>::match( iterator first, iterator end )
	{
		MatchResult<iterator> m0 = _npN::match( first, end );
		MatchResult<iterator> m1 = re::match( m0.pos, end );

		if( m1.success )
			return m1;
		else
			return MatchResult<iterator>::Failed(first);
	}


	template<typename re,typename Action> 
	template<typename iterator, typename stack>
	MatchResult<iterator> re_plus<re,Action>::match( iterator first, iterator end, stack& s )
	{
		return re_follow< re, re_star<re>, Action >::match(first,end,s);
	}


	template<typename re,typename Action> 
	template<typename iterator>
	MatchResult<iterator> re_plus<re,Action>::match( iterator first, iterator end )
	{
		return re_follow< re, re_star<re>, Action >::match(first,end);
	}


	template<typename	re, unsigned int N, unsigned int M,typename Action>
	template<typename iterator, typename stack>
	MatchResult<iterator> re_repeat<re,N,M,Action>::match( iterator first, iterator end, stack& s )
	{
		SU_COMPILE_TIME_ASSERT( N <= M );
		int sz = s.size();

		MatchResult<iterator> m( first, true );
		uint n;
		for( n=0 ; n<M ; n++ )
		{
			m = re::match( m.pos, end, s );
			if( !m.success )	break;
		}

		if( n < N )
		{
			s.resize( sz );
			return MatchResult<iterator>::Failed(first);
		}
		else
		{
			Action::apply( s, sz, first, m.pos );
			return MatchResult<iterator>::Successed(m.pos);
		}
	}


	template<typename	re, unsigned int N, unsigned int M,typename Action>
	template<typename iterator>
	MatchResult<iterator> re_repeat<re,N,M,Action>::match( iterator first, iterator end )
	{
		SU_COMPILE_TIME_ASSERT( N <= M );

		MatchResult<iterator> m( first, true );
		uint n;
		for( n=0 ; n<M ; n++ )
		{
			m = re::match( m.pos, end );
			if( !m.success )	break;
		}

		if( n < N )
			return MatchResult<iterator>::Failed(first);
		else
			return MatchResult<iterator>::Successed(m.pos);
	}


}};




#endif //_su_rexpi_h_






