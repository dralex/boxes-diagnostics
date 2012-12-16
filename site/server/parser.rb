
def unzip_buffer(zipbuf)
	return zipbuf
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
		  :addduration => nil,
		  :addoper => nil,
		  :edit2duration => nil,
		  :search2duration => nil,
		  :search2oper => nil,
		  :add2duration => nil,
		  :add2oper => nil,
		  :editlog => nil,
		  :searchlog => nil,
		  :addlog => nil,
		  :edit2log => nil,
		  :search2log => nil,
		  :add2log => nil,
		  :comments => nil}
	current_text = nil
	buffer.each_line { |line|
		line.strip!
		next if line.empty?
		if line =~ /^AppVersion: (.*)$/
			result[:appversion] = $1
		elsif line =~ /^SysInfo: (.*)$/
			result[:sysinfo] = $1
		elsif line =~ /^LocalTime: (.*)$/	
			result[:localtime] = $1.to_i
		elsif line =~ /^School: (.*)$/
			result[:school] = $1
		elsif line =~ /^Class: (.*)$/
			result[:class] = $1
		elsif line =~ /^Name: (.*)$/
			result[:name] = $1
		elsif line =~ /^Background: (.*)$/
			result[:background] = $1
		elsif line =~ /^EditDuration: (.*)$/
			result[:editduration] = $1.to_i
		elsif line =~ /^SearchDuration: (.*)$/
			result[:searchduration] = $1.to_i
		elsif line =~ /^SearchOperations: (.*)$/
			result[:searchoper] = $1.to_i
		elsif line =~ /^AddDuration: (.*)$/
			result[:addduration] = $1.to_i
		elsif line =~ /^AddOperations: (.*)$/
			result[:addoper] = $1.to_i
		elsif line =~ /^Edit2Duration: (.*)$/
			result[:edit2duration] = $1.to_i
		elsif line =~ /^Search2Duration: (.*)$/
			result[:search2duration] = $1.to_i
		elsif line =~ /^Search2Operations: (.*)$/
			result[:search2oper] = $1.to_i
		elsif line =~ /^Add2Duration: (.*)$/
			result[:add2duration] = $1.to_i
		elsif line =~ /^Add2Operations: (.*)$/
			result[:add2oper] = $1.to_i
		elsif line == 'EditLog:'
			result[:editlog] = ''
			current_text = :editlog
		elsif line == 'SearchLog:'
			result[:searchlog] = ''
			current_text = :searchlog
		elsif line == 'AddLog:'
			result[:addlog] = ''
			current_text = :addlog
		elsif line == 'Edit2Log:'
			result[:edit2log] = ''
			current_text = :edit2log
		elsif line == 'Search2Log:'
			result[:search2log] = ''
			current_text = :search2log
		elsif line == 'Add2Log:'
			result[:add2log] = ''
			current_text = :add2log
		elsif line == 'Comments:'
			result[:comments] = ''
			current_text = :comments
		elsif not current_text.nil?
			result[current_text] += line + "\n"
		else 
			puts "Bad line #{line}"
			next
		end
	}
	return result
end