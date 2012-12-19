CREATE TABLE diagn_results (
	id		INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	servertime	TIMESTAMP(8) DEFAULT NOW(),
	diagnid		VARCHAR(64),
	appversion	VARCHAR(16),
	sysinfo		VARCHAR(128),
	remotetime	INT,
	school		VARCHAR(32),
	class		VARCHAR(32),
	name		VARCHAR(64),
	background	TEXT,
	editlog		LONGTEXT,
	editdump	TEXT,
	editduration	SMALLINT,
	searchlog	TEXT,
	searchoper	SMALLINT,
	searchduration	SMALLINT,
	search2log	TEXT,
	search2oper	SMALLINT,
	search2duration	SMALLINT,
	addlog		TEXT,
	addoper		SMALLINT,
	addduration	SMALLINT,
	edit2log	TEXT,
	edit2dump	TEXT,
	edit2duration	SMALLINT,
	search3log	TEXT,
	search3oper	SMALLINT,
	search3duration	SMALLINT,
	add2log		TEXT,
	add2oper	SMALLINT,
	add2duration	SMALLINT,
	comments	TEXT,
	remoteaddr	VARCHAR(64)		
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
