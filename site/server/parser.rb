
def unzip_buffer(zipbuf)
	return zipbuf
end

def compare_versions(v1, v2)
	return 0 if v1 == v2
	return nil if v1.nil? or v2.nil?
	return nil unless v1 =~ /(\d+)\.(\d+)/
	v1major, v1minor = $1, $2
	return nil unless v2 =~ /(\d+)\.(\d+)/
	v2major, v2minor = $1, $2
	if v1major < v2major
		return 1
	elsif v1major > v2major
		return -1
	else
		if v1minor < v2minor
			return 1
		elsif v1minor > v2minor
			return -1
		else
			return 0
		end
	end
end

def parse_buffer(buffer)
	result = {:appversion => nil,
		:sysinfo => nil,
		:localtime => nil,
		:school => nil,
		:class => nil,
		:name => nil,
		:background => nil,
		:editduration => nil,
		:searchduration => nil,
		:searchoper => nil,
		:searchargs => nil,
		:addduration => nil,
		:addoper => nil,
		:addargs => nil,
		:edit2duration => nil,
		:search2duration => nil,
		:search2oper => nil,
		:search2args => nil,
		:add2duration => nil,
		:add2oper => nil,
		:add2args => nil,
		:search3duration => nil,
		:search3oper => nil,
		:search3args => nil,
		:search4duration => nil,
		:search4oper => nil,
		:search4args => nil,
		:editlog => nil,
		:editdump => nil,
		:searchlog => nil,
		:addlog => nil,
		:edit2log => nil,
		:edit2dump => nil,
		:search2log => nil,
		:add2log => nil,
		:search3log => nil,
		:search4log => nil,
		:comments => nil,
		:diagnid => nil,
		:books => nil,
		:startduration => nil,
		:language => nil
	}
	current_text = nil
	buffer.each_line { |line|
		line.gsub!(/\r?\n$/,'')
		next if line.empty?
		if line =~ /^AppVersion: (.*)$/
			result[:appversion] = $1
		elsif line =~ /^DiagnID: (.*)$/
			result[:diagnid] = $1
		elsif line =~ /^SysInfo: (.*)$/
			result[:sysinfo] = $1
		elsif line =~ /^LocalTime: (.*)$/	
			result[:localtime] = $1.to_i
		elsif line =~ /^School:(.*)$/
			result[:school] = $1.strip
		elsif line =~ /^Class:(.*)$/
			result[:class] = $1.strip
		elsif line =~ /^Name: (.*)$/
			result[:name] = $1
		elsif line =~ /^Language: (.*)$/
			result[:language] = $1
		elsif line =~ /^Background: (.*)$/
			result[:background] = $1
			current_text = :background
		elsif line =~ /^StartDuration: (.*)$/
			result[:startduration] = $1.to_i
		elsif line =~ /^EditDuration: (.*)$/
			result[:editduration] = $1.to_i
		elsif line =~ /^SearchArgs: (.*)$/
			result[:searchargs] = $1.strip
		elsif line =~ /^SearchDuration: (.*)$/
			result[:searchduration] = $1.to_i
		elsif line =~ /^SearchOperations: (.*)$/
			result[:searchoper] = $1.to_i
		elsif line =~ /^AddArgs: (.*)$/
			result[:addargs] = $1.strip
		elsif line =~ /^AddDuration: (.*)$/
			result[:addduration] = $1.to_i
		elsif line =~ /^AddOperations: (.*)$/
			result[:addoper] = $1.to_i
		elsif line =~ /^Edit2Duration: (.*)$/
			result[:edit2duration] = $1.to_i
		elsif line =~ /^Search2Args: (.*)$/
			result[:searchargs] = $1.strip
		elsif line =~ /^Search2Duration: (.*)$/
			result[:search2duration] = $1.to_i
		elsif line =~ /^Search2Operations: (.*)$/
			result[:search2oper] = $1.to_i
		elsif line =~ /^Search3Args: (.*)$/
			result[:searchargs] = $1.strip
		elsif line =~ /^Search3Duration: (.*)$/
			result[:search3duration] = $1.to_i
		elsif line =~ /^Search3Operations: (.*)$/
			result[:search3oper] = $1.to_i
		elsif line =~ /^Search4Args: (.*)$/
			result[:search4args] = $1.strip
		elsif line =~ /^Search4Duration: (.*)$/
			result[:search4duration] = $1.to_i
		elsif line =~ /^Search4Operations: (.*)$/
			result[:search4oper] = $1.to_i
		elsif line =~ /^Add2Args: (.*)$/
			result[:add2args] = $1.strip
		elsif line =~ /^Add2Duration: (.*)$/
			result[:add2duration] = $1.to_i
		elsif line =~ /^Add2Operations: (.*)$/
			result[:add2oper] = $1.to_i
		elsif line =~ /^Books: (.*)$/ and compare_versions('1.3', result[:appversion]) >= 0
			result[:books] = "#{$1}\n"
			current_text = :books
		elsif line == 'EditLog:'
			result[:editlog] = ''
			current_text = :editlog
		elsif line == 'EditDump:'
			result[:editdump] = ''
			current_text = :editdump
		elsif line == 'SearchLog:'
			result[:searchlog] = ''
			current_text = :searchlog
		elsif line == 'AddLog:'
			result[:addlog] = ''
			current_text = :addlog
		elsif line == 'Edit2Log:'
			result[:edit2log] = ''
			current_text = :edit2log
		elsif line == 'Edit2Dump:'
			result[:edit2dump] = ''
			current_text = :edit2dump
		elsif line == 'Search2Log:'
			result[:search2log] = ''
			current_text = :search2log
		elsif line == 'Add2Log:'
			result[:add2log] = ''
			current_text = :add2log
		elsif line == 'Search3Log:'
			result[:search3log] = ''
			current_text = :search3log
		elsif line == 'Search4Log:'
			result[:search4log] = ''
			current_text = :search3log
		elsif line == 'Comments:'
			result[:comments] = ''
			current_text = :comments
		elsif line =~ /^NetworkError:/
			next
		elsif line =~ /^WrongNetworkResult:/
			current_text = :wrong_result
			next
		elsif not current_text.nil?
			next if current_text == :wrong_result
			result[current_text] += line + "\n"
		else 
			puts "Bad line #{line}"
			next
		end
	}
	return result
end
