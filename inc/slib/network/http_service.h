#ifndef CHECKHEADER_SLIB_NETWORK_HTTP_SERVICE
#define CHECKHEADER_SLIB_NETWORK_HTTP_SERVICE

#include "http_common.h"

#include "socket_address.h"

SLIB_NETWORK_NAMESPACE_BEGIN

class HttpService;
class HttpServiceConnection;

class SLIB_EXPORT HttpServiceContext : public Object, public HttpRequestHeader, public HttpResponseHeader, public HttpOutputBuffer
{
	SLIB_DECLARE_OBJECT(HttpServiceContext, Object)
protected:
	HttpServiceContext();
	
public:
	static Ref<HttpServiceContext> create(const Ref<HttpServiceConnection>& connection);

public:
	SLIB_INLINE Memory getRawRequestHeader() const
	{
		return m_requestHeader;
	}

	SLIB_INLINE sl_uint64 getRequestContentLength() const
	{
		return m_requestContentLength;
	}

	SLIB_INLINE Memory getRequestBody() const
	{
		return m_requestBody;
	}
	
public:
	SLIB_INLINE sl_uint64 getResponseContentLength() const
	{
		return getOutputLength();
	}
	
public:
	Ref<HttpService> getService();
	
	Ref<HttpServiceConnection> getConnection();
	
	Ref<AsyncStream> getIO();
	
	Ref<AsyncLoop> getAsyncLoop();
	
	const SocketAddress& getLocalAddress();
	
	const SocketAddress& getRemoteAddress();
	
public:
	SLIB_BOOLEAN_PROPERTY_INLINE(ClosingConnection);
	
	SLIB_BOOLEAN_PROPERTY_INLINE(ProcessingByThread);

protected:
	HttpHeaderReader m_requestHeaderReader;
	SafeMemory m_requestHeader;
	sl_uint64 m_requestContentLength;
	MemoryBuffer m_requestBodyBuffer;
	SafeMemory m_requestBody;

private:
	WeakRef<HttpServiceConnection> m_connection;

	friend class HttpServiceConnection;
};

class SLIB_EXPORT HttpServiceConnection : public Object, public IAsyncStreamListener, public IAsyncOutputListener, public IClosable
{
protected:
	HttpServiceConnection();
	~HttpServiceConnection();
	
public:
	static Ref<HttpServiceConnection> create(HttpService* service, AsyncStream* io);

public:
	// override
	void close();
	
	void start(const void* data = sl_null, sl_uint32 size = 0);

public:
	void sendResponse(const Memory& mem);
	
	void sendResponseAndRestart(const Memory& mem);
	
	void sendResponseAndClose(const Memory& mem);

	
	void sendResponse_BadRequest();
	
	void sendResponse_ServerError();
	
	void sendConnectResponse_Successed();
	
	void sendConnectResponse_Failed();
	
	void sendProxyResponse_Failed();

public:
	SLIB_PROPERTY_INLINE(SocketAddress, LocalAddress)
	
	SLIB_PROPERTY_INLINE(SocketAddress, RemoteAddress)
	
	SLIB_REF_PROPERTY_INLINE(Referable, ProxyObject)
	
	SLIB_REF_PROPERTY_INLINE(Referable, UserObject)

public:
	SLIB_INLINE Ref<AsyncStream> getIO()
	{
		return m_io;
	}
	
	SLIB_INLINE Ref<HttpService> getService()
	{
		return m_service;
	}

	SLIB_INLINE Ref<HttpServiceContext> getCurrentContext()
	{
		return m_contextCurrent;
	}

protected:
	WeakRef<HttpService> m_service;
	Ref<AsyncStream> m_io;
	Ref<AsyncOutput> m_output;

	SafeRef<HttpServiceContext> m_contextCurrent;

	sl_bool m_flagClosed;
	Memory m_bufRead;
	sl_bool m_flagReading;

protected:
	void _read();
	
	void _processInput(const void* data, sl_uint32 size);
	
	void _processContext(Ref<HttpServiceContext> context);

protected:
	// override
	void onRead(AsyncStream* stream, void* data, sl_uint32 sizeRead, const Referable* ref, sl_bool flagError);
	
	// override
	void onAsyncOutputComplete(AsyncOutput* output);
	
	// override
	void onAsyncOutputError(AsyncOutput* output);
};


class SLIB_EXPORT HttpServiceConnectionProvider : public Object
{
public:
	virtual void release() = 0;
	
public:
	Ref<HttpService> getService();
	
	void setService(const Ref<HttpService>& service);
	
private:
	WeakRef<HttpService> m_service;
};


class SLIB_EXPORT IHttpServiceProcessor
{
public:
	virtual sl_bool onHttpRequest(HttpServiceContext* context) = 0;
};

class SLIB_EXPORT HttpServiceParam
{
public:
	IPAddress addressBind;
	sl_uint16 port;

	sl_uint32 maxThreadsCount;
	sl_bool flagProcessByThreads;

	sl_bool flagUseResource;
	String prefixResource;

	sl_uint64 maxRequestHeadersSize;
	sl_uint64 maxRequestBodySize;

	sl_bool flagLogDebug;

public:
	HttpServiceParam();
	
};

class SLIB_EXPORT HttpService : public Object
{
	SLIB_DECLARE_OBJECT(HttpService, Object)
protected:
	HttpService();
	~HttpService();
	
public:
	static Ref<HttpService> create(const HttpServiceParam& param);
	
public:
	sl_bool start(const HttpServiceParam& param);
	
	void release();
	
public:
	virtual Ref<HttpServiceConnection> addConnection(const Ref<AsyncStream>& stream, const SocketAddress& remoteAddress, const SocketAddress& localAddress);
	
	virtual void closeConnection(HttpServiceConnection* connection);

	// called before processing body, returns true if the service is trying to process the connection itself.
	virtual sl_bool preprocessRequest(HttpServiceContext* context);

	// called after inputing body
	virtual void processRequest(HttpServiceContext* context);

	virtual sl_bool processResource(HttpServiceContext* context, String path);

public:
	SLIB_INLINE sl_bool isRunning()
	{
		return m_flagRunning;
	}

	SLIB_INLINE const Ref<AsyncLoop>& getAsyncLoop()
	{
		return m_loop;
	}
	
	SLIB_INLINE const HttpServiceParam& getParam()
	{
		return m_param;
	}

public:
	void addProcessor(const Ptr<IHttpServiceProcessor>& processor);
	
	void removeProcessor(const Ptr<IHttpServiceProcessor>& processor);
	
	List< Ptr<IHttpServiceProcessor> > getProcessors();

	
	void addConnectionProvider(const Ref<HttpServiceConnectionProvider>& provider);

	void removeService(const Ref<HttpServiceConnectionProvider>& provider);

	
	sl_bool addHttpService(const SocketAddress& addr);

	sl_bool addHttpService(sl_uint32 port = 80);

	sl_bool addHttpService(const IPAddress& addr, sl_uint32 port = 80);
	
protected:
	Ref<AsyncLoop> m_loop;
	sl_bool m_flagRunning;
	
	HashMap< HttpServiceConnection*, Ref<HttpServiceConnection> > m_connections;

	CList< Ptr<IHttpServiceProcessor> > m_processors;
	CList< Ref<HttpServiceConnectionProvider> > m_connectionProviders;

	HttpServiceParam m_param;
};

SLIB_NETWORK_NAMESPACE_END

#endif
