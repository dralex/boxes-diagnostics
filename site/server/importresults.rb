#!/usr/bin/ruby

require 'parser'
require 'db_engine'

ARGV.each { |a|
	print "#{a}... "
	f = File.open(a, 'r')
	buffer = f.read
	r = parse_buffer(buffer)
	db_insert('localhost', r)
	f.close
	puts "DONE"
}
