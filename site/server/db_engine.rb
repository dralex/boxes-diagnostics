
require 'mysql'

DB_HOST = 'localhost'
DB_USER = 'diagn'
DB_PASSWORD = 'passw0rd'
DB_DATABASE = 'diagn_results'

def db_insert(d)
	begin	
		c = Mysql.new(DB_HOST, DB_USER, DB_PASSWORD, DB_DATABASE)
		c.autocommit(false)
		c.query('SET CHARACTER SET utf8')
		req = c.prepare('INSERT INTO diagn_results (appversion, sysinfo, remotetime, school,' +
						'class, name, background, editlog, editduration, searchlog, searchoper,' +
						'searchduration, addlog, addoper, addduration, edit2log, edit2duration,' +
						'search2log, search2oper, search2duration, add2log, add2oper, add2duration,' +
						'comments, search3oper, search3duration, search3log, editdump, edit2dump) ' + 
						'values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)')
		req.execute(d[:appversion], d[:sysinfo], d[:localtime], d[:school], d[:class], d[:name],
					d[:background], d[:editlog], d[:editduration], d[:searchlog], d[:searchoper],
					d[:searchduration], d[:addlog], d[:addoper], d[:addduration], d[:edit2log],
					d[:edit2duration], d[:search2log], d[:search2oper], d[:search2duration],
					d[:add2log], d[:add2oper], d[:add2duration], d[:comments], d[:search3oper],
					d[:search3duration], d[:search3log], d[:editdump], d[:edit2dump])
		c.commit
	rescue Mysql::Error => e
		puts e
		c.rollback
	ensure
		req.close if req
		c.close if c
	end
end
