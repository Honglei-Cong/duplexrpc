#include "stdafx.h"
#include "client.h"
#include <random>
#include <ctime>

#define DRI_IS_CLIENT
#include "interface.h"
using namespace boost;

class Client : public rpc::ClientPeer<Interface>
{
	shared_ptr<Interface> conn;
public:
	void on_connected( const boost::system::error_code& err, const shared_ptr<Interface>& conn_ )
	{
		if (err)
		{
			cout<<"���ó���"<<boost::system::system_error(err).what()<<endl;
		}
		else
		{
			cout<<"���ӳɹ���"<<endl;
			conn = conn_;
		}
	}

	void on_connection_in_place()
	{
		conn->set_my_name( "���", &Client::on_set_my_name );
		conn->get_my_name( &Client::on_get_my_name );
		conn->set_my_name( "���1", &Client::on_set_my_name );
		conn->get_my_name( &Client::on_get_my_name );
		conn->set_my_name( "���2", &Client::on_set_my_name );
		conn->get_my_name( &Client::on_get_my_name );
		conn->set_my_name( "���3", &Client::on_set_my_name );
		conn->get_my_name( &Client::on_get_my_name );
		conn->set_my_name( "���4", &Client::on_set_my_name );
		conn->get_my_name( &Client::on_get_my_name );
		conn->set_my_name( "���5", &Client::on_set_my_name );
		conn->get_my_name( &Client::on_get_my_name );
		conn->who_am_i("abc", "123", &Client::on_who_am_i );
		conn->who_am_i("abcd", "1234", &Client::on_who_am_i );
		conn->who_am_i("abce", "12345", &Client::on_who_am_i );
		conn->who_am_i("abcf", "123456", &Client::on_who_am_i );
		conn->who_am_i("abcg", "1234567", &Client::on_who_am_i );
		conn->who_am_i("abch", "12345678", &Client::on_who_am_i );
	}

	static void on_set_my_name( const remote_call_error& err )
	{
		if (err)
		{
			cout<<"����on_set_my_nameʧ�ܣ�"<<err.msg<<endl;
		}
		else
		{
			cout<<"����on_set_my_name�ɹ���"<<endl;
		}
	}

	static void on_get_my_name( const remote_call_error& err, const string& s )
	{
		if (err)
		{
			cout<<"����on_get_my_nameʧ�ܣ�"<<err.msg<<endl;
		}
		else
		{
			cout<<"����on_get_my_name�ɹ������أ�"<<endl<<s<<endl;
		}
	}

	static void on_who_am_i( const remote_call_error& err, const string& s)
	{
		if (err)
		{
			cout<<"����who_am_i(\"abc\")ʧ�ܣ�"<<err.msg<<endl;
		}
		else
		{
			cout<<"����who_am_i(\"abc\")�ɹ������أ�"<<endl<<s<<endl;
		}
	}

} ;


void start_client()
{
	Interface::connect<Client>( "localhost", 7844 );
}