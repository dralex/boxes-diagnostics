#!/usr/bin/ruby

require 'parser'
require 'db_engine'

if(ENV['REQUEST_METHOD'] != 'POST' || ENV['CONTENT_TYPE'] != 'application/octet-stream')
	require 'cgi'
	cgi = CGI.new
	cgi.out('status' => 'BAD_REQUEST',
		'connection' => 'close') {'Request Error, sorry'}
else 
	print("Content-type: text/plain\n\n")
	data = STDIN.read
	buffer = unzip_buffer(data)
	r = parse_buffer(buffer)
	db_insert(r)
        puts 'OK'
end
