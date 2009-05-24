/********************************************************************
	created:	2008/05/25
	created:	25:5:2008   14:24
	filename: 	\rpc\rpc_common.hpp
	author:		����
	purpose:	
	Copyright (C) 2008 , all rights reserved.
    *********************************************************************/
#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/asio.hpp>

/**
* �������ռ�����˱�rpcʵ�ֵ����д���.
*/
namespace rpc{

	namespace detail{
		//���紫�䲻��Ҫͷ����archive::serialization����
		const unsigned int archive_flags = boost::archive::no_header;
	}
    typedef boost::archive::binary_iarchive iarchive;
    typedef boost::archive::binary_oarchive oarchive;

    /** �Ҳ���ָ����RPC����id�� */
    class func_id_not_found : public std::exception{};
    /** RPC����id�Ѵ��ڡ� */
    class func_id_already_exists : public std::exception{};

	/** ʹ�õĺ����ࡣ */
#ifndef RPC_FUNCTION_CLASS
# define RPC_FUNCTION_CLASS boost::function
#endif

	/** ʹ�õ�shared_ptr�ࡣ */
#ifndef RPC_SHARED_PTR
# define RPC_SHARED_PTR boost::shared_ptr
#endif

	/** ʹ�õ�shared_ptr�ࡣ */
#ifndef RPC_WEAK_PTR
# define RPC_WEAK_PTR boost::weak_ptr
#endif

	/** ʹ�õ�enable_shared_from_this�ࡣ */
#ifndef RPC_ENABEL_SHARED_FROM_THIS
# define RPC_ENABEL_SHARED_FROM_THIS boost::enable_shared_from_this
#endif

	/** ʹ�õ�bind�ࡣ */
#ifndef RPC_BIND_FUNCTION
# define RPC_BIND_FUNCTION boost::bind
#endif

	/** ʹ�õ�_1�� */
#ifndef RPC__1
# define RPC__1 _1
#endif
#ifndef RPC__2
# define RPC__2 _2
#endif
#ifndef RPC__3
# define RPC__3 _3
#endif
#ifndef RPC__4
# define RPC__4 _4
#endif
#ifndef RPC__5
# define RPC__5 _5
#endif

//	/** һ�����ݰ�����󳤶ȡ�Ĭ��Ϊ128KB�� */
//#ifndef RPC_PACKET_MAX_LENGTH
//# define RPC_PACKET_MAX_LENGTH 128*1024
//#endif

    /**
    * Զ�̵��õ�״̬��
    */
    class remote_call_error{
    public:
		enum error_code : unsigned char{
            succeeded,              /**< Զ�̵��óɹ���ɡ� */
            connection_error,       /**< ���ӳ��� */
            func_not_found,         /**< �Ҳ���Ҫ���õ�Զ�̺����� */
            func_call_error,        /**< ���ҵ�Զ�̺����������ù����г��ִ��� */
            return_type_mismatch,   /**< ���ҵ�Զ�̺�����������ֵ���Ͳ��ԡ� */
            no_rpc_service,         /**< �Է���peerû���ṩrpc���� */
        };
        remote_call_error():code(succeeded){}
        remote_call_error(error_code code_, std::string msg_):code(code_), msg(msg_){}

		typedef void (*unspecified_bool_type)();
		static void unspecified_bool_true() {}

		operator unspecified_bool_type() const  // true if error
		{ 
			return code==succeeded ? 0 : unspecified_bool_true;
		}

        remote_call_error(const remote_call_error& r){
            code = r.code;
            msg = r.msg;
        }
        void operator = (const remote_call_error& r){
            code = r.code;
            msg = r.msg;
        }
		
		std::string what() const { return msg; }

        error_code		code;
        std::string		msg;

		template<class Archive>	void serialize( Archive & ar, const unsigned int version )
		{
			ar & BOOST_SERIALIZATION_NVP(code);
			if ( code!=succeeded )
			{
				ar & BOOST_SERIALIZATION_NVP(msg);
			}
		}
    };

    typedef boost::uint64_t packet_id;

    /**
    * RPC����ͷ����������RPC����,Ҳ�����Ƿ��ص�RPC�����
	* �����򷵻�ֵ�ڴ˰�֮��
    */
    class packet : private boost::noncopyable{
    public:
        packet():is_request(true) {}
        packet(bool is_request_, const remote_call_error& err_, const std::string& func_id_)
            :is_request(is_request_), err(err_), func_id(func_id_){}
        packet(const packet& r)
		{
            is_request = r.is_request;
            err = r.err;
            func_id = r.func_id;
        }
        void operator = (const packet& r)
		{
            is_request = r.is_request;
            err = r.err;
            func_id = r.func_id;
        }

        bool				is_request;
        remote_call_error	err;
        std::string			func_id;

		template<class Archive>	void serialize( Archive & ar, const unsigned int version )
		{
			ar & BOOST_SERIALIZATION_NVP(is_request);
			if ( !is_request )
			{
				ar & BOOST_SERIALIZATION_NVP(err);
			}
			if ( is_request )
			{
				ar & BOOST_SERIALIZATION_NVP(func_id);
			}
		}
    };

	typedef RPC_SHARED_PTR<packet> packet_ptr;

	template<class T>
    inline void empty_func(T& data_to_keep){}
}//namespace rpc