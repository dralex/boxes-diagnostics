
require 'mysql'

DB_HOST = 'localhost'
DB_USER = 'diagn'
DB_PASSWORD = 'passw0rd'
DB_DATABASE = 'diagn_results'

def db_list()
	results = []
	begin	
		c = Mysql.new(DB_HOST, DB_USER, DB_PASSWORD, DB_DATABASE)
		c.query('SET CHARACTER SET utf8')
		res = c.query("select id, school, class, name, appversion, " + 
					  " (IF(startduration is NULL, 0, startduration) + " +
					  "editduration + IF(searchduration is NULL, 0, searchduration) + " + 
					  "IF(search2duration is NULL, 0, search2duration) + " +
					  "IF(addduration is NULL, 0, addduration) + " +
					  "IF(edit2duration is NULL, 0, edit2duration) + " +
					  "IF(search3duration is NULL, 0, search3duration) + " +
					  "IF(add2duration is NULL, 0, add2duration))" + 
					  " as totaltime, isnull(add2duration) as broken from diagn_results order by servertime")
		res.num_rows.times {
			row = res.fetch_row
			results.push(row)
		}
	rescue Mysql::Error => e
		puts e
	ensure
		c.close if c
	end	
	return results
end

def db_fetch(id)
	r = {}
	begin	
		c = Mysql.new(DB_HOST, DB_USER, DB_PASSWORD, DB_DATABASE)
		c.query('SET CHARACTER SET utf8')
		res = c.query('select id, servertime, appversion, sysinfo, remotetime, school,' +
					  'class, name, background, editlog, editduration, searchlog, searchoper,' +
					  'searchduration, addlog, addoper, addduration, edit2log, edit2duration,' +
					  'search2log, search2oper, search2duration, add2log, add2oper, add2duration,' +
					  'comments, search3oper, search3duration, search3log, editdump, edit2dump, ' + 
					  "remoteaddr, diagnid, books, startduration, language from diagn_results where id = #{id}")
		row = res.fetch_row
		r[:id] = row[0]
		r[:servertime] = row[1]
		r[:appversion] = row[2]
		r[:sysinfo] = row[3]
		r[:localtime] = row[4]
		r[:school] = row[5]
		r[:class] = row[6]
		r[:name] = row[7]
		r[:background] = row[8]
		r[:editlog] = row[9]
		r[:editduration] = row[10]
		r[:searchlog] = row[11]
		r[:searchoper] = row[12]
		r[:searchduration] = row[13]
		r[:addlog] = row[14]
		r[:addoper] = row[15]
		r[:addduration] = row[16]
		r[:edit2log] = row[17]
		r[:edit2duration] = row[18]
		r[:search2log] = row[19]
		r[:search2oper] = row[20]
		r[:search2duration] = row[21]
		r[:add2log] = row[22]
		r[:add2oper] = row[23]
		r[:add2duration] = row[24]
		r[:comments] = row[25]
		r[:search3oper] = row[26]
		r[:search3duration] = row[27]
		r[:search3log] = row[28]
		r[:editdump] = row[29]
		r[:edit2dump] = row[30]
		r[:remoteaddr] = row[31]
		r[:diagnid] = row[32]
		r[:books] = row[33]
		r[:startduration] = row[34]
		r[:language] = row[35]
	rescue Mysql::Error => e
		return e.to_s
	ensure
		c.close if c
	end	
	return r
end

def db_insert(r_addr, d)
	begin	
		c = Mysql.new(DB_HOST, DB_USER, DB_PASSWORD, DB_DATABASE)
		c.query('SET CHARACTER SET utf8')
		available = false
		unless d[:diagnid].nil?
			res = c.query("SELECT id FROM diagn_results WHERE diagnid = '#{d[:diagnid]}'")
			available = res.num_rows != 0
		end
		unless available then
			c.autocommit(false)
			req = c.prepare('INSERT INTO diagn_results (appversion, sysinfo, remotetime, school,' +
							'class, name, background, editlog, editduration, searchlog, searchoper,' +
							'searchduration, addlog, addoper, addduration, edit2log, edit2duration,' +
							'search2log, search2oper, search2duration, add2log, add2oper, add2duration,' +
							'comments, search3oper, search3duration, search3log, editdump, edit2dump, ' + 
							'remoteaddr, diagnid, books, startduration, language) ' +
							'values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)')
			req.execute(d[:appversion], d[:sysinfo], d[:localtime], d[:school], d[:class], d[:name],
						d[:background], d[:editlog], d[:editduration], d[:searchlog], d[:searchoper],
						d[:searchduration], d[:addlog], d[:addoper], d[:addduration], d[:edit2log],
						d[:edit2duration], d[:search2log], d[:search2oper], d[:search2duration],
						d[:add2log], d[:add2oper], d[:add2duration], d[:comments], d[:search3oper],
						d[:search3duration], d[:search3log], d[:editdump], d[:edit2dump], r_addr,
						d[:diagnid], d[:books], d[:startduration], d[:language])
			c.commit
		else
			puts "DiagnID '#{d[:diagnid]}' already available"
		end
	rescue Mysql::Error => e
		puts e
		c.rollback
	ensure
		req.close if req
		c.close if c
	end
end
