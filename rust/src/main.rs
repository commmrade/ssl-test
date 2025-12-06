use std::{io::Write, net::{SocketAddr, TcpStream}};

use openssl::ssl::{Ssl, SslContext, SslMethod};
use trust_dns_resolver::{Resolver, config::{ResolverConfig, ResolverOpts}};

fn main() {
    let resolver = Resolver::new(ResolverConfig::default(), ResolverOpts::default()).unwrap();
    let rslv_resp = resolver.lookup_ip("www.google.com").unwrap();
    let addr = rslv_resp.iter().next().unwrap();

    let sock = TcpStream::connect(SocketAddr::new(addr, 443)).unwrap();

    let ssl_ctx = SslContext::builder(SslMethod::tls_client()).unwrap().build();
    let ssl = Ssl::new(&ssl_ctx).unwrap();
    let mut stream = ssl.connect(sock).unwrap();

    let get_req: &str = "GET / HTTP/1.1\r\n\
        Host: google.com\r\n\
        Connection: close\r\n\
        User-Agent: curl/8.0\r\n\
        \r\n";
        
    let mut resp = Vec::new();
    resp.resize(4096, 0);

    _ = stream.write_all(get_req.as_bytes()).unwrap();
    let rd_bytes = stream.ssl_read(&mut resp).unwrap();
    println!("Read {} bytes, {}", rd_bytes, String::from_utf8(resp).unwrap());
}
