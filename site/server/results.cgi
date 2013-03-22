#!/usr/bin/ruby
# -*- coding: utf-8 -*-

require 'db_engine'
require 'cgi'
require 'parser'

def dur2str(t)
	s = ''
	t = t.to_i
	if t > 3600
		s += "#{t / 3600} ч. "
		t = t % 3600
	end
	if t > 60
		s += "#{t / 60} мин "
		t = t % 60
	end
	s += "#{t % 60} сек"
	s
end

def time2str(t)
	Time.at(t.to_i).strftime('%Y-%m-%d %H:%M:%S')
end

def dump2str(tree)
	res = ''
	tree.each_line { |l|
		l.gsub!(/\r?\n$/,'')
		if l =~ /^Root/
			res += "Корень\n"
		elsif l =~ /^(\s*)B\{.*\}\{\}\{(.*)\}/
			res += "#{$1}Ящик #{$2}\n"
		elsif l =~ /^(\s*)F\{.*\}\{(.*)\}\{(.*)\}/
			if $3.length == 0
				res += "#{$1}#{$2.split(':').join(' ')}\n"
			else
				res += "#{$1}#{$3}\n"
			end
		end
	}
	return res
end

$dict = {}
def b2str(bookid)
	b = $dict[bookid]
	if b.size > 2
		if b.size == 4
			b[3]
		elsif b[0].strip.empty?
			"#{b[1]} - #{b[2]}"
		else 
			"#{b[0]} #{b[1]} - #{b[2]}"
		end
	else
		b[0]
	end
end

def findtree(tree, id)
	$debug += "find tree #{tree} #{id}\n"
	tree.each_index {|idx|
		t = tree[idx]
		if t[0] == id 
			return tree, idx, ''
		elsif t.size == 3
			p, i = findtree(t[2], id)
			unless p.nil?
				return p, i
			end
		end
	}
	return nil, nil
end

def inserttree(tree, idx, target)
	if tree.size == idx 
		tree.push(target)
	else
		tree[idx, tree.size - 1] = [target] + tree[idx, tree.size - 1]
	end	
end

def rebuild_books(rbooks, rlog)
	books = []
	log_array = rlog.split("\n")
	log_idx = 0
	if rbooks.nil?
		log_array[log_idx] =~ /^Books:\s*(\d+)/
		booknum = $1.to_i
		log_idx += 1
		booknum.times { |i|
			s = log_array[log_idx].strip
			if s =~ /^\{(.*)\}\{(.*):(.*):(.*)\}$/
				books.push([$1, $2, $3, $4])
			elsif s =~ /^\d+:\{(.*)\}\{(.*):(.*):(.*)\}$/
				books.push([$1, $2, $3, $4])
			end
			log_idx += 1
		}
	else
		book_array = rbooks.split("\n")
		booknum = book_array[0].strip.to_i
		book_idx = 1
		booknum.times { |i|
			s = book_array[book_idx].strip
			s =~ /^\d+:\{(.*)\}\{(.*):(.*):(.*)\}$/
			books.push([$1, $2, $3, $4])
			book_idx += 1
		}
	end

	$dict = {}
	books.each { |b|
		$dict[b[0]] = b[1, 3]
	}

	return books
end

def rebuild_editlog(rbooks, rlog, rdump)
	$debug = ''
	books = []
	history = ''
	log_array = rlog.split("\n")
	log_idx = 0
	if rbooks.nil?
		log_array[log_idx] =~ /^Books:\s*(\d+)/
		booknum = $1.to_i
		log_idx += 1
		booknum.times { |i|
			s = log_array[log_idx].strip
			if s =~ /^\{(.*)\}\{(.*):(.*):(.*)\}$/
				books.push([$1, $2, $3, $4])
			elsif s =~ /^\d+:\{(.*)\}\{(.*):(.*):(.*)\}$/
				books.push([$1, $2, $3, $4])
			end
			log_idx += 1
		}
	else
		book_array = rbooks.split("\n")
		booknum = book_array[0].strip.to_i
		book_idx = 1
		booknum.times { |i|
			s = book_array[book_idx].strip
			s =~ /^\d+:\{(.*)\}\{(.*):(.*):(.*)\}$/
			books.push([$1, $2, $3, $4])
			book_idx += 1
		}
	end
	
	books.each { |b|
		$dict[b[0]] = b[1, 3]
	}

	tree = books.dup
	
	while log_idx < log_array.size
		l = log_array[log_idx].strip
		if l =~ /^Reor?dered\s+([^\s]*)\s+(\d+)->(\d+)$/
			parentid = $1.strip
			frompos, topos = $2.to_i, $3.to_i
			if parentid.empty?
				history += "В корне "
				curdir = tree
			else
				parentid =~ /^\{(.*)\}/
				parentid = $1
				history += "В ящике '#{parentid}'"
				curdir = findtree(parentid)[0]
			end
			history += "'#{b2str(curdir[frompos][0])}'"
			if topos == curdir.size
				history += " переместить в конец списка"
			else
				history += " переместить перед "
			end
			history += "'#{b2str(curdir[topos][0])}'\n"
			
			book = curdir.delete_at(frompos)
			inserttree(curdir, topos, book)
		elsif l =~ /^T\s+(\d+)$/
			time = $1.to_i
			history += "Прошло: #{dur2str(time)}\n"
		elsif l =~ /^Remove\s+(.*)$/
			removeid = $1.strip
			history += "Удалить '#{b2str(removeid)}'\n"
			parent, idx = findtree(tree, removeid)
			parent.delete_at(idx)
		elsif l =~ /^Books returned to root:\s+(\d+)$/
			booksnum = $1.to_i
			log_idx += 1
			booksnum.times { 
				bookid = log_array[log_idx].strip
				tree.push([bookid] + $dict[bookid])
				log_idx += 1
			}
			next
		elsif l =~ /^New box\s+\{(.*)\}\{(.*)\}\s+at\s+(.*):(\d+)$/
			boxid = $1
			box = [$2, []]
			$dict[boxid] = box
			history += "Создать ящик '#{$2}' "
			if $3.strip.empty? 
				parentdir = tree
				history += "в корне\n"
			else
				parentid = $3
				parentid =~ /^\{(.*)\}/
				parentid = $1
				parentdir = findtree(tree, $3)[0]
				history += "в ящике '#{b2str(boxid)}'\n"
			end
			idx = $4.to_i
			inserttree(parentdir, idx, [boxid] + box)
		elsif l =~ /^Moved\s+\{(.*)\}->(.*):?(\d+)?$/
			fromid = $1
			toid = $2
			toindex = $3
			$debug += "from #{fromid} to #{toid}\n"
			fromroot, fromindex = findtree(tree, fromid)
			target = fromroot.delete_at(fromindex)
			history += "'#{b2str(target[0])}' переместить из '#{b2str(fromid)}' "
			if toid.strip.empty?
				history += 'в корень '
				toroot = tree
			else
				toid =~ /^\{(.*)\}/
				toid = $1				
				toroot = findtree(tree, toid)
				history += "в ящик '#{b2str(toid)}' "
			end
			if toindex.nil? or toindex.strip.empty? 
				toindex = toroot.size 
			else
				toindex = toindex.to_i
			end
			if toindex == toroot.size
				history += " переместить в конец списка\n"
			else
				history += " переместить перед '#{b2str(toroot[toindex][0])}'\n"
			end
			inserttree(toroot, toindex, target)
		end
		log_idx += 1
	end

	return tree, history, books
end

def rebuild_searchaddlog(rlog)
	history = ''
	return 'нет' if rlog.nil?
	log_array = rlog.split("\n")
	log_idx = 0
	while log_idx < log_array.size
		l = log_array[log_idx].strip
		if l =~ /^Start search$/
			history += "Начало поиска\n"
		elsif l =~ /^Start add$/
			history += "Начало добавления\n"
		elsif l =~ /^Found$/
			history += "Книга найдена\n"
		elsif l =~ /^O\s+(\d+)(\s+\d+)?$/
			t = $2
			if t.nil?
				history += "Число операций: #{$1}\n"
			else
				history += "Прошло: #{dur2str(t.strip)}\n"
				history += "Число операций: #{$1}\n"
			end
		elsif l == '>'
			history += "[Дальше]\n"
		elsif l == '<'
			history += "[Раньше]\n"
		elsif l == '>|'
			history += "[В конец]\n"
		elsif l == '<|'
			history += "[В начало]\n"
		elsif l =~ /^Current\s+\{.*\}\{(.*)\}$/
			history += "Текущий элемент: #{$1}\n"
		elsif l =~ /^Clicked\s+\{.*\}\{(.*)\}$/
			history += "Выбран элемент: #{$1}\n"
		elsif l =~ /^New book\s+\{.*\}\{(.*)\}\s+at\s+(\{.*\})?:\d+/
			history += "Книга для добавления: #{$1}\n"
		elsif l =~ /New box\s+\{.*\}\{(.*)\}\s+at\s+(\{.*\})?:\d+/
			history += "Имя нового ящика: #{$1}\n"
		elsif l == 'Open'
			history += "[Открыть ящик]\n"
		elsif l == 'Close'
			history += "[Закрыть ящик]\n"
		elsif l == 'Inserted after'
			history += "[Добавить книгу после текущего элемента]\n"
		elsif l == 'Inserted before'
			history += "[Добавить книгу перед текущим элементом]\n"
		elsif l == 'Current empty box'
			history += "Текущий элемент: пустой ящик\n"
		elsif l == 'Add box'
			history += "[Добавить ящик и открыть его]\n"
		elsif l == 'Search cancelled'
			history += "Поиск отменен\n"
		elsif l == 'Addition cancelled'
			history += "Добавление отменено\n"
		elsif l =~ /^T\s+(\d+)$/
			next
		else 
			history += "#{l}\n"
		end
		log_idx += 1
	end
	return history
end

def books2str(books)
	s = ''
	books.each { |b|
		s += "#{b2str(b[0])}\n"
	}
	s
end

def tree2str(tree, indent = "\t")
	s = "Корень:\n"
	tree.each { |t|
		if t.size > 2
			s += "#{indent}#{b2str(t[0])}\n"
		else
			s += "#{indent}Ящик #{b2str(t[0])} [\n"
			s += tree2str(t[1], indent + "\t")
			s += "#{indent}]\n"
		end
	}
	s
end

cgi = CGI.new('html4')

if cgi.has_key? 'id'	
	cgi.out {
		cgi.html {
			id = cgi['id'].to_i
			r = db_fetch(id)
			if r.kind_of? String 
				cgi.body { 'db error: ' + r }
			else
				cgi.head { cgi.title { "Результаты N#{r[:id]} - #{r[:name]}" } } +
				cgi.body {
					begin
						s = "<h1>Результаты диагностики N#{r[:id]} - #{r[:name]}</h1>"

						s += "<h2>Программа</h2>"
						s += "<ul><li><strong>Версия:</strong> #{r[:appversion]}</li>"
						s += "<li><strong>Операционная система:</strong> #{r[:sysinfo]}</li>"
						s += "<li><strong>Адрес:</strong> #{r[:remoteaddr]}</li></ul>"
						if r[:language] == nil
							s += "<li><strong>Язык:</strong> Русский (старый)</li>"
						else
							s += "<li><strong>Язык:</strong> #{r[:language]}</li>"
						end

						s += "<h2>Участник</h2>"
						s += "<ul><li><strong>Школа:</strong> #{r[:school]}</li>"
						s += "<li><strong>Класс:</strong> #{r[:class]}</li>"
						s += "<li><strong>Имя:</strong> #{r[:name]}</li>"
						s += "<li><strong>Опыт работы на компьютере:</strong> #{r[:background]}</li></ul>"
						
						s += "<h2>Время</h2>"
						s += "<ul><li><strong>Время начала (локально):</strong> #{time2str(r[:localtime])}</li>"
						s += "<li><strong>Время загрузки данных (сервер):</strong> #{r[:servertime]}</li>"
						s += "<li><strong>Продолжительность:</strong></ul>"
						s += ("<table width='80%' border='1'><tr><th>Начало</th><th>Редактирование 1</th>" + 
							  "<th>Поиск 1</th><th>Поиск 2</th><th>Добавление 1</th>" +
							  "<th>Редактирование 2</th><th>Поиск 3</th><th>Добавление 2</th><th>Всего</th></tr>")
						s += ("<tr><td>#{dur2str(r[:startduration])}</td><td>#{dur2str(r[:editduration])}</td><td>#{dur2str(r[:searchduration])}</td>" +
							  "<td>#{dur2str(r[:search2duration])}</td><td>#{dur2str(r[:addduration])}</td><td>#{dur2str(r[:edit2duration])}</td>" +
							  "<td>#{dur2str(r[:search3duration])}</td><td>#{dur2str(r[:add2duration])}</td><td><strong>" + 
							  dur2str(r[:startduration].to_i + r[:editduration].to_i + r[:searchduration].to_i + r[:search2duration].to_i +
									  r[:addduration].to_i + r[:edit2duration].to_i + r[:search3duration].to_i + r[:add2duration].to_i) + "</strong></td>")
						s += '</table>'
						
						s += "<h2>Число операций</h2>"
						s += ("<table width='50%' border='1'><tr><th>Поиск 1</th><th>Поиск 2</th><th>Добавление 1</th>" +
							  "<th>Поиск 3</th><th>Добавление 2</th></tr>")
						s += ("<tr><td>#{r[:searchoper]}</td><td>#{r[:search2oper]}</td><td>#{r[:addoper]}</td>" +
							  "<td>#{r[:search3oper]}</td><td>#{r[:add2oper]}</td>")
						s += '</table>'

						books = rebuild_books(r[:books], r[:editlog])

						s += "<h2>Книги</h2>"
						s += "<p><strong>Число книг:</strong> #{books.size}"
						s += "<p><strong>Книги:</strong>"
						s += cgi.pre { books2str(books) }

						s += "<h2>Первый этап редактирования</h2>"
						s += "<p><strong>Время редактирования:</strong> #{dur2str(r[:editduration])}"
						s += "<p><strong>Последовательность шагов:</strong>"
					
						rebuilded = compare_versions('1.4', r[:appversion]) >= 0 

						if rebuilded
							tree, history, books = rebuild_editlog(r[:books], r[:editlog], r[:editdump])
							rebuilded = true
							s += cgi.pre { r[:editlog] }
							s += cgi.pre { history }
						else 
							rebuilded = false
							if r[:editlog].nil? or r[:editlog].strip.empty?
								s += cgi.pre { 'нет' } 
							else
								s += cgi.pre { r[:editlog] }
							end
						end

						s += "<p><strong>Дерево на конец редактирования:</strong>"
						
						if rebuilded 
							s += cgi.pre { r[:editdump] } 
							s += cgi.pre { tree2str(tree) }							
						else 						
							s += cgi.pre { r[:editdump] } 
						end

						s += "<h2>Поиск 1</h2>"
						
						if compare_versions('1.5', r[:appversion]) >= 0 
							s += "<p><strong>Вопрос для поиска:</strong> Найдите книгу с названием &quot;Приключения Гекльберри Финна&quot;"
						else
							s += "<p><strong>Книга для поиска:</strong> Марк Твен - Приключения Гекльберри Финна"
						end

						s += "<p><strong>Время поиска:</strong> #{dur2str(r[:searchduration])}"
						s += "<p><strong>Число операций:</strong> #{r[:searchoper]}"
						s += '<p><strong>Последовательность поиска:</strong>'
						s += cgi.pre { rebuild_searchaddlog(r[:searchlog]) }

						s += "<h2>Поиск 2</h2>"

						if compare_versions('1.5', r[:appversion]) >= 0 
							s += "<p><strong>Вопрос для поиска:</strong> Найдите книгу Стендаля"
						else
							s += "<p><strong>Книга для поиска:</strong> Николай Гоголь - Вечера на хуторе близ Диканьки"
						end

						s += "<p><strong>Время поиска:</strong> #{dur2str(r[:search2duration])}"
						s += "<p><strong>Число операций:</strong> #{r[:search2oper]}"
						s += '<p><strong>Последовательность поиска:</strong>'
						s += cgi.pre { rebuild_searchaddlog(r[:search2log]) }
						
						s += "<h2>Добавление 1</h2>"						

						s += "<p><strong>Книга для добавления:</strong> "
						if compare_versions('1.5', r[:appversion]) >= 0 
							s += "Аркадий и Борис Стругацкие - Жук в муравейнике"
						else
							s += "Аркадий и Борис Стругацкие - Жук в муравейнике"
						end
						
						s += "<p><strong>Время добавления:</strong> #{dur2str(r[:addduration])}"
						s += "<p><strong>Число операций:</strong> #{r[:addoper]}"
						s += '<p><strong>Последовательность добавления:</strong>'
						s += cgi.pre { rebuild_searchaddlog(r[:addlog]) }

						s += "<h2>Второй этап редактирования</h2>"
						s += "<p><strong>Время редактирования:</strong> #{dur2str(r[:edit2duration])}"
						s += "<p><strong>Последовательность шагов:</strong>"
						if r[:edit2log].nil? or r[:edit2log].strip.empty?
							s += cgi.pre { 'нет' } 
						else
							s += cgi.pre { r[:edit2log] } 
						end
						s += "<p><strong>Дерево на конец редактирования:</strong>"
						s += cgi.pre { r[:edit2dump] } 

						s += "<h2>Поиск 3</h2>"

						if compare_versions('1.5', r[:appversion]) >= 0 
							s += "<p><strong>Вопрос для поиска:</strong> Найдите книгу &quot;Золотой теленок&quot;"
						else
							s += "<p><strong>Книга для поиска:</strong> Александр Пушкин - Повести Белкина"
						end

						s += "<p><strong>Время поиска:</strong> #{dur2str(r[:search3duration])}"
						s += "<p><strong>Число операций:</strong> #{r[:search3oper]}"
						s += '<p><strong>Последовательность поиска:</strong>'
						s += cgi.pre { rebuild_searchaddlog(r[:search3log]) }
						
						s += "<h2>Добавление 2</h2>"						

						s += "<p><strong>Книга для добавления:</strong> "
						if compare_versions('1.5', r[:appversion]) >= 0 
							s += "Carl Marx - Capital"
						else
							s += "Carl Marx - Capital"
						end

						s += "<p><strong>Время добавления:</strong> #{dur2str(r[:add2duration])}"
						s += "<p><strong>Число операций:</strong> #{r[:add2oper]}"
						s += '<p><strong>Последовательность добавления:</strong>'
						s += cgi.pre { rebuild_searchaddlog(r[:add2log]) }

						s
					rescue Exception => e
						s + cgi.pre { $debug } + "<p>Error: #{e.message}" + "<p>#{e.backtrace.join('<br>')}"
					end
				}
			end
		}
	}
else
	cgi.out {
		cgi.html {
			cgi.head { cgi.title { 'Результаты диагностики' } } +
			cgi.body {
				list = db_list()
				s = '<h1>Результаты диагностики</h1>'
				s += '<table width="70%" border="1">'
				s += '<tr><th>Детали</th><th>Школа</th><th>Класс</th><th>Имя</th><th>Версия</th><th>Завершена</th><th>Общее время</th></tr>'
				i = 1
				list.each { |r|
					if r[6] == '0'
						finished = '<strong>да</strong>'
					else
						finished = 'нет'
					end
					s += "<tr><td><a href='results.cgi?id=#{r[0]}'>#{i}</a></td><td>#{r[1]}</td>"
					s += "<td>#{r[2]}</td><td>#{r[3]}</td><td>#{r[4]}</td><td>#{finished}</td><td>#{dur2str(r[5])}</td></tr>"
					i += 1
				}
				s += '</table>'
				s
			}
		}
	}
end
