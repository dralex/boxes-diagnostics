CREATE TABLE diagn_results (
	id		INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	servertime	TIMESTAMP(8) DEFAULT NOW(),
	diagnid		VARCHAR(64), INDEX(diagnid),
	appversion	VARCHAR(16),
	sysinfo		VARCHAR(128),
	language	VARCHAR(32),
	remotetime	INT,
	school		VARCHAR(32),
	class		VARCHAR(32),
	name		VARCHAR(64),
	background	TEXT,
	startduration	SMALLINT,
	books		TEXT,
	editlog		LONGTEXT,
	editdump	TEXT,
	editduration	SMALLINT,
	searchargs	TEXT,
	searchlog	TEXT,
	searchoper	SMALLINT,
	searchduration	SMALLINT,
	search2args	TEXT,
	search2log	TEXT,
	search2oper	SMALLINT,
	search2duration	SMALLINT,
	search3args	TEXT,
	search3log	TEXT,
	search3oper	SMALLINT,
	search3duration	SMALLINT,
	addargs		TEXT,
	addlog		TEXT,
	addoper		SMALLINT,
	addduration	SMALLINT,
	edit2log	TEXT,
	edit2dump	TEXT,
	edit2duration	SMALLINT,
	search4args	TEXT,
	search4log	TEXT,
	search4oper	SMALLINT,
	search4duration	SMALLINT,
	add2args	TEXT,
	add2log		TEXT,
	add2oper	SMALLINT,
	add2duration	SMALLINT,
	comments	TEXT,
	remoteaddr	VARCHAR(64)		
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
