/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

%token SQL_TYPE
%token SQL_ABS
%token ACOS
%token AMPERSAND
%token SQL_ABSOLUTE
%token ACOS
%token ADA
%token ADD
%token ADD_DAYS
%token ADD_HOURS
%token ADD_MINUTES
%token ADD_MONTHS
%token ADD_SECONDS
%token ADD_YEARS
%token ALL
%token ALLOCATE
%token ALTER
%token AND
%token ANY
%token ARE
%token AS
%token ASIN
%token ASC
%token ASCII
%token ASSERTION
%token ATAN
%token ATAN2
%token AUTHORIZATION
%token AUTO_INCREMENT
%token AVG
%token BEFORE
%token SQL_BEGIN
%token BETWEEN
%token BIGINT
%token BINARY
%token BIT
%token BIT_LENGTH
%token BREAK
%token BY
%token CASCADE
%token CASCADED
%token CASE
%token CAST
%token CATALOG
%token CEILING
%token CENTER
%token SQL_CHAR
%token CHAR_LENGTH
%token CHARACTER_STRING_LITERAL
%token CHECK
%token CLOSE
%token COALESCE
%token COBOL
%token COLLATE
%token COLLATION
%token COLUMN
%token COMMIT
%token COMPUTE
%token CONCAT
%token CONNECT
%token CONNECTION
%token CONSTRAINT
%token CONSTRAINTS
%token CONTINUE
%token CONVERT
%token CORRESPONDING
%token COS
%token COT
%token COUNT
%token CREATE
%token CURDATE
%token CURRENT
%token CURRENT_DATE
%token CURRENT_TIME
%token CURRENT_TIMESTAMP
%token CURTIME
%token CURSOR
%token DATABASE
%token SQL_DATE
%token DATE_FORMAT
%token DATE_REMAINDER
%token DATE_VALUE
%token DAY
%token DAYOFMONTH
%token DAYOFWEEK
%token DAYOFYEAR
%token DAYS_BETWEEN
%token DEALLOCATE
%token DEC
%token DECLARE
%token DEFAULT
%token DEFERRABLE
%token DEFERRED
%token SQL_DELETE
%token DESC
%token DESCRIBE
%token DESCRIPTOR
%token DIAGNOSTICS
%token DICTIONARY
%token DIRECTORY
%token DISCONNECT
%token DISPLACEMENT
%token DISTINCT
%token DOMAIN_TOKEN
%token SQL_DOUBLE
%token DOUBLE_QUOTED_STRING
%token DROP
%token ELSE
%token END
%token END_EXEC
%token EQUAL
%token ESCAPE
%token EXCEPT
%token SQL_EXCEPTION
%token EXEC
%token EXECUTE
%token EXISTS
%token EXP
%token EXPONENT
%token EXTERNAL
%token EXTRACT
%token SQL_FALSE
%token FETCH
%token FIRST
%token SQL_FLOAT
%token FLOOR
%token FN
%token FOR
%token FOREIGN
%token FORTRAN
%token FOUND
%token FOUR_DIGITS
%token FROM
%token FULL
%token GET
%token GLOBAL
%token GO
%token GOTO
%token GRANT
%token GREATER_OR_EQUAL
%token GREATER_THAN
//conflict %token GROUP
%token HAVING
%token HOUR
%token HOURS_BETWEEN
%token IDENTITY
%token IFNULL
%token SQL_IGNORE
%token IMMEDIATE
%token SQL_IN
%token INCLUDE
%token INDEX
%token INDICATOR
%token INITIALLY
%token INNER
%token INPUT
%token INSENSITIVE
%token INSERT
%token INTEGER
%token INTERSECT
%token INTERVAL
%token INTO
%token IS
%token ISOLATION
%token JOIN
%token JUSTIFY
%token KEY
%token LANGUAGE
%token LAST
%token LCASE
%token LEFT
%token LENGTH
%token LESS_OR_EQUAL
%token LESS_THAN
%token LEVEL
%token LIKE
%token LINE_WIDTH
%token LOCAL
%token LOCATE
%token LOG
%token SQL_LONG
%token LOWER
%token LTRIM
%token LTRIP
%token MATCH
%token SQL_MAX
%token MICROSOFT
%token SQL_MIN
%token MINUS
%token MINUTE
%token MINUTES_BETWEEN
%token MOD
%token MODIFY
%token MODULE
%token MONTH
%token MONTHS_BETWEEN
%token MUMPS
%token NAMES
%token NATIONAL
%token NCHAR
%token NEXT
%token NODUP
%token NONE
%token NOT
%token NOT_EQUAL
%token NOW
%token SQL_NULL
%token NULLIF
%token NUMERIC
%token OCTET_LENGTH
%token ODBC
%token OF
%token SQL_OFF
%token SQL_ON
%token ONLY
%token OPEN
%token OPTION
%token OR
%token ORDER
%token OUTER
%token OUTPUT
%token OVERLAPS
%token PAGE
%token PARTIAL
%token SQL_PASCAL
%token PERSISTENT
%token CQL_PI
%token PLI
%token POSITION
%token PRECISION
%token PREPARE
%token PRESERVE
%token PRIMARY
%token PRIOR
%token PRIVILEGES
%token PROCEDURE
%token PRODUCT
%token PUBLIC
%token QUARTER
%token QUIT
%token RAND
%token READ_ONLY
%token REAL
%token REFERENCES
%token REPEAT
%token REPLACE
%token RESTRICT
%token REVOKE
%token RIGHT
%token ROLLBACK
%token ROWS
%token RPAD
%token RTRIM
%token SCHEMA
%token SCREEN_WIDTH
%token SCROLL
%token SECOND
%token SECONDS_BETWEEN
%token SELECT
%token SEQUENCE
%token SETOPT
%token SET
%token SHOWOPT
%token SIGN
%token INTEGER_CONST
%token REAL_CONST
%token SIN
%token SQL_SIZE
%token SMALLINT
%token SOME
%token SPACE
%token SQL
%token SQL_TRUE
%token SQLCA
%token SQLCODE
%token SQLERROR
%token SQLSTATE
%token SQLWARNING
%token SQRT
%token STDEV
%token SUBSTRING
%token SUM
%token SYSDATE
%token SYSDATE_FORMAT
%token SYSTEM
%token TABLE
%token TAN
%token TEMPORARY
%token THEN
%token THREE_DIGITS
%token TIME
%token TIMESTAMP
%token TIMEZONE_HOUR
%token TIMEZONE_MINUTE
%token TINYINT
%token TO
%token TO_CHAR
%token TO_DATE
%token TRANSACTION
%token TRANSLATE
%token TRANSLATION
%token TRUNCATE
%token GENERAL_TITLE
%token TWO_DIGITS
%token UCASE
%token UNION
%token UNIQUE
%token SQL_UNKNOWN
//%token UNSIGNED_INTEGER
%token UPDATE
%token UPPER
%token USAGE
%token USER
%token IDENTIFIER
%token IDENTIFIER_DOT_ASTERISK
%token ERROR_DIGIT_BEFORE_IDENTIFIER
%token USING
%token VALUE
%token VALUES
%token VARBINARY
%token VARCHAR
%token VARYING
%token VENDOR
%token VIEW
%token WEEK
%token WHEN
%token WHENEVER
%token WHERE
%token WHERE_CURRENT_OF
%token WITH
%token WORD_WRAPPED
%token WORK
%token WRAPPED
%token YEAR
%token YEARS_BETWEEN

%token '-' '+'
%token '*'
%token '%'
%token '@'
%token ';'
%token ','
%token '.'
%token '$'
//%token '<'
//%token '>'
%token '(' ')'
%token '?'
%token '\''
%token '/'

%type <stringValue> IDENTIFIER
%type <stringValue> IDENTIFIER_DOT_ASTERISK
%type <stringValue> CHARACTER_STRING_LITERAL
%type <stringValue> DOUBLE_QUOTED_STRING

/*
%type <field> ColExpression
%type <field> ColView
*/
%type <expr> ColExpression
%type <expr> ColWildCard
//%type <expr> ColView
%type <expr> ColItem
%type <exprList> ColViews
%type <expr> aExpr
%type <exprList> aExprList
%type <expr> WhereClause
%type <expr> FlatTable
%type <exprList> Tables
%type <exprList> FlatTableList
%type <querySchema> SelectStatement
%type <querySchema> Select
/*todo : list*/
%type <querySchema> StatementList
/*todo: not onlu select*/
%type <querySchema> Statement

%type <colType> SQL_TYPE
%type <integerValue> INTEGER_CONST
%type <realValue> REAL_CONST
/*%type <integerValue> SIGNED_INTEGER */

%{
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <assert.h>

#ifdef _WIN32
# include <malloc.h>
#endif

#include <qobject.h>
#include <kdebug.h>
#include <klocale.h>
#include <qptrlist.h>
#include <qcstring.h>

#include <connection.h>
#include <queryschema.h>
#include <field.h>
#include <tableschema.h>

#include "parser.h"
//#include "sqlparser.h"
#include "sqltypes.h"

//	using namespace std;
	using namespace KexiDB;

#define YY_NO_UNPUT
#define YYSTACK_USE_ALLOCA 1
#define YYMAXDEPTH 255

Parser *parser;
KexiDB::Field *field;
bool requiresTable;
QPtrList<KexiDB::Field> fieldList;
//	QPtrList<KexiDB::TableSchema> tableList;
//	QDict<KexiDB::TableSchema> tableDict;
//	KexiDB::TableSchema *dummy = 0;
int current = 0;
QString ctoken = "";

int yyparse();
int yylex();
void tokenize(const char *data);

void yyerror(const char *str)
{
	kdDebug() << "error: " << str << endl;
	kdDebug() << "at character " << current << " near tooken " << ctoken << endl;
	parser->setOperation(KexiDB::Parser::OP_Error);

	if (parser->error().type().isEmpty() 
		&& (strlen(str)==0 
		|| qstricmp(str, "syntax error")==0 || qstricmp(str, "parse error")==0))
	{
		kdDebug() << parser->statement() << endl;
		QString ptrline = "";
		for(int i=0; i < current; i++)
			ptrline += " ";

		ptrline += "^";

		kdDebug() << ptrline << endl;

		//lexer may add error messages
		QString lexerErr = parser->error().error();
		if (!lexerErr.isEmpty())
			lexerErr.prepend(": ");
			
		KexiDB::ParserError err(i18n("Syntax Error"), i18n("Syntax Error near \"%1\"").arg(ctoken)+lexerErr, ctoken, current);
		parser->setError(err);
	}
}

void setError(const QString& errName, const QString& errDesc)
{
	parser->setError( KexiDB::ParserError(errName, errDesc, ctoken, current) );
	yyerror(errName.latin1());
}

void setError(const QString& errDesc)
{
	setError("", errDesc);
}

void tableNotFoundError(const QString& tableName)
{
	setError( i18n("Table not found"), i18n("Unknown table \"%1\"").arg(tableName) );
}

/* this is better than assert() */
#define IMPL_ERROR(errmsg) setError("Implementation error", errmsg)

bool parseData(KexiDB::Parser *p, const char *data)
{
/* todo: remove dummy */
//		if (!dummy)
			//dummy = new KexiDB::TableSchema();
/* todo: make this REENTRANT */
	parser = p;
	parser->clear();
	field = 0;
	fieldList.clear();
	requiresTable = false;

	if (!data) {
		KexiDB::ParserError err(i18n("Error"), i18n("No query specified"), ctoken, current);
		parser->setError(err);
		yyerror("");
		parser = 0;
		return false;
	}

	tokenize(data);
	if (!parser->error().type().isEmpty()) {
		parser = 0;
		return false;
	}
	yyparse();

	bool ok = true;
	if(parser->operation() == KexiDB::Parser::OP_Select)
	{
		kdDebug() << "parseData(): ok" << endl;
//			kdDebug() << "parseData(): " << tableDict.count() << " loaded tables" << endl;
/*			KexiDB::TableSchema *ts;
			for(QDictIterator<KexiDB::TableSchema> it(tableDict); KexiDB::TableSchema *s = tableList.first(); s; s = tableList.next())
			{
				kdDebug() << "  " << s->name() << endl;
			}*/
/*removed
			KexiDB::Field::ListIterator it = parser->select()->fieldsIterator();
			for(KexiDB::Field *item; (item = it.current()); ++it)
			{
				if(tableList.findRef(item->table()) == -1)
				{
					KexiDB::ParserError err(i18n("Field List Error"), i18n("Unknown table '%1' in field list").arg(item->table()->name()), ctoken, current);
					parser->setError(err);

					yyerror("fieldlisterror");
					ok = false;
				}
			}*/
			//take the dummy table out of the query
//			parser->select()->removeTable(dummy);
	}
	else {
		ok = false;
	}

//		tableDict.clear();
	parser = 0;
	return ok;
}

	
/* Adds \a column to \a querySchema. \a column can be in a form of
 table.field, tableAlias.field or field
*/
bool addColumn( QuerySchema *querySchema, BaseExpr* column, 
	const QDict< QValueList<int> > &repeatedTablesAndAliases )
{
	VariableExpr *v_e = dynamic_cast<VariableExpr*>(column);
	if (column->exprClass() != KexiDBExpr_Variable || !v_e) {
		//err
		return false;
	}
	QString varName = v_e->name;
	kdDebug() << "found variable name: " << varName << endl;
	int dotPos = varName.find('.');
	QString tableName, fieldName;
//TODO: shall we also support db name?
	if (dotPos>0) {
		tableName = varName.left(dotPos);
		fieldName = varName.mid(dotPos+1);
	}
	if (tableName.isEmpty()) {//fieldname only
		fieldName = varName;
		if (fieldName=="*") {
//			querySchema->addAsterisk( new KexiDB::QueryAsterisk(parser->select()) );
			querySchema->addAsterisk( new KexiDB::QueryAsterisk(querySchema) );
		}
		else {
			//find first table that has this field
			Field *firstField = 0;
			for (TableSchema::ListIterator it(*querySchema->tables()); it.current(); ++it) {
				Field *f = it.current()->field(fieldName);
				if (f) {
					if (!firstField) {
						firstField = f;
					} else if (f->table()!=firstField->table()) {
						//ambiguous field name
						setError(i18n("Ambiguous field name"), 
							i18n("Both table \"%1\" and \"%2\" have defined \"%3\" field. "
								"Use \"<tableName>.%4\" notation to specify table name.")
								.arg(firstField->table()->name()).arg(f->table()->name())
								.arg(fieldName).arg(fieldName));
						return false;
					}
				}
			}
			if (!firstField) {
					setError(i18n("Field not found"), 
						i18n("Table containing \"%1\" field not found").arg(fieldName));
					return false;
			}
			//ok
			querySchema->addField(firstField);
		}
	}
	else {//table.fieldname or tableAlias.fieldname
		tableName = tableName.lower();
		KexiDB::TableSchema *ts = querySchema->table( tableName );
		if (ts) {//table.fieldname
			//check if "table" is covered by an alias
			const QValueList<int> tPositions = querySchema->tablePositions(tableName);
			QValueList<int>::ConstIterator it = tPositions.begin();
			QCString tableAlias;
			bool covered = true;
			for (; it!=tPositions.end() && covered; ++it) {
				tableAlias = querySchema->tableAlias(*it);
				if (tableAlias.isEmpty() || tableAlias.lower()==tableName.latin1())
					covered = false; //uncovered
				kdDebug() << " --" << "covered by " << tableAlias << " alias" << endl;
			}
			if (covered) {
				setError(i18n("Could not access the table directly using its name"), 
					i18n("Table \"%1\" is covered by aliases. Instead of \"%2\", "
					"you can write \"%3\"").arg(tableName)
					.arg(tableName+"."+fieldName).arg(tableAlias+"."+fieldName.latin1()));
				return false;
			}
		}
		
		int tablePosition = -1;
		if (!ts) {//try to find tableAlias
			tablePosition = querySchema->tablePositionForAlias( tableName.latin1() );
			if (tablePosition>=0) {
				ts = querySchema->tables()->at(tablePosition);
				if (ts)
					kdDebug() << " --it's a tableAlias.name" << endl;
			}
		}


		if (ts) {
			QValueList<int> *positionsList = repeatedTablesAndAliases[ tableName ];
			if (!positionsList) {
				IMPL_ERROR(tableName + "." + fieldName + ", !positionsList ");
				return false;
			}

			if (fieldName=="*") {
				if (positionsList->count()>1) {
					setError(i18n("Ambiguous \"%1.*\" expression").arg(tableName),
						i18n("More than one \"%1\" table or alias defined").arg(tableName));
					return false;
				}
//				querySchema->addAsterisk( new KexiDB::QueryAsterisk(parser->select(), ts) );
				querySchema->addAsterisk( new KexiDB::QueryAsterisk(querySchema, ts) );
			}
			else {
				kdDebug() << " --it's a table.name" << endl;
				KexiDB::Field *realField = ts->field(fieldName);
				if (realField) {
					// check if table or alias is used twice and both have the same column
					// (so the column is ambiguous)
					int numberOfTheSameFields = 0;
					for (QValueList<int>::iterator it = positionsList->begin();
						it!=positionsList->end();++it)
					{
						KexiDB::TableSchema *otherTS = querySchema->tables()->at(*it);
						if (otherTS->field(fieldName))
							numberOfTheSameFields++;
						if (numberOfTheSameFields>1) {
							setError(i18n("Ambiguous \"%1.%2\" expression").arg(tableName).arg(fieldName),
								i18n("More than one \"%1\" table or alias defined containing \"%2\" field").arg(tableName).arg(fieldName));
							return false;
						}
					}

					querySchema->addField(realField, tablePosition);
				}
				else {
					setError(i18n("Field not found"), i18n("Table \"%1\" has no \"%2\" field")
						.arg(tableName).arg(fieldName));
					return false;
				}
			}
		}
		else {
			tableNotFoundError(tableName);
			return false;
		}
	}
	return true;
}

	
	extern "C"
	{
		int yywrap()
		{
			return 1;
		}
	}

#if 0
	struct yyval
	{
		QString parserUserName;
		int integerValue;
		KexiDBField::ColumnType coltype;
	}
#endif
%}

%union {
	char stringValue[255];
	int integerValue;
	struct realType realValue;
	KexiDB::Field::Type colType;
	KexiDB::Field *field;
	KexiDB::BaseExpr *expr;
	KexiDB::NArgExpr *exprList;
	KexiDB::ConstExpr *constExpr;
	KexiDB::QuerySchema *querySchema;
}

//%left '=' NOT_EQUAL '>' GREATER_OR_EQUAL '<' LESS_OR_EQUAL LIKE '%' NOT
//%left '+' '-'
//%left ASTERISK SLASH

/* precedence: lowest to highest */
%left		UNION EXCEPT
%left		INTERSECT
%left		OR
%left		AND
%right	NOT
//%right		'='
//%nonassoc	'<' '>'
//%nonassoc '=' '<' '>' "<=" ">=" "<>" ":=" LIKE ILIKE SIMILAR
%nonassoc '=' LESS_THAN GREATER_THAN LESS_OR_EQUAL GREATER_OR_EQUAL NOT_EQUAL 
%nonassoc SQL_IN LIKE ILIKE SIMILAR
//%nonassoc	LIKE ILIKE SIMILAR
//%nonassoc	ESCAPE
//%nonassoc	OVERLAPS
%nonassoc	BETWEEN
//%nonassoc	IN_P
//%left		POSTFIXOP		// dummy for postfix Op rules 
//%left		Op OPERATOR		// multi-character ops and user-defined operators 
//%nonassoc	NOTNULL
//%nonassoc	ISNULL
//%nonassoc	IS NULL_P TRUE_P FALSE_P UNKNOWN // sets precedence for IS NULL, etc 
%left		'+' '-'
%left		'*' '/' '%'
%left		'^'
// Unary Operators 
//%left		AT ZONE			// sets precedence for AT TIME ZONE
//%right		UMINUS
%left		'[' ']'
%left		'(' ')'
//%left		TYPECAST
%left		'.'

/*
 * These might seem to be low-precedence, but actually they are not part
 * of the arithmetic hierarchy at all in their use as JOIN operators.
 * We make them high-precedence to support their use as function names.
 * They wouldn't be given a precedence at all, were it not that we need
 * left-associativity among the JOIN rules themselves.
 */
/*%left		JOIN UNIONJOIN CROSS LEFT FULL RIGHT INNER_P NATURAL
*/
%%

TopLevelStatement :
StatementList
{
//todo: multiple statements
//todo: not only "select" statements
	parser->setOperation(KexiDB::Parser::OP_Select);
	parser->setQuerySchema($1);
}
;

StatementList:
Statement ';' StatementList
{
//todo: multiple statements
}
| Statement
| Statement ';'
{
	$$ = $1;
}
;

/*		Statement CreateTableStatement 		{ YYACCEPT; }
	| Statement SelectStatement 		{  }
*/
Statement :
CreateTableStatement
{
YYACCEPT;
}
| SelectStatement
{
	$$ = $1;
}
;

CreateTableStatement :
CREATE TABLE IDENTIFIER
{
	parser->setOperation(KexiDB::Parser::OP_CreateTable);
	parser->createTable($3);
}
'(' ColDefs ')'
;

ColDefs:
ColDefs ',' ColDef|ColDef
{
}
;

ColDef:
IDENTIFIER ColType
{
	kdDebug() << "adding field " << $1 << endl;
	field->setName($1);
	parser->table()->addField(field);

//	delete field;
	field = 0;
}
| IDENTIFIER ColType ColKeys
{
	kdDebug() << "adding field " << $1 << endl;
	field->setName($1);
	parser->table()->addField(field);

//	if(field->isPrimaryKey())
//		parser->table()->addPrimaryKey(field->name());

//	delete field;
//	field = 0;
}
;

ColKeys:
ColKeys ColKey|ColKey
{
}
;

ColKey:
PRIMARY KEY
{
	field->setPrimaryKey(true);
	kdDebug() << "primary" << endl;
}
| NOT SQL_NULL
{
	field->setNotNull(true);
	kdDebug() << "not_null" << endl;
}
| AUTO_INCREMENT
{
	field->setAutoIncrement(true);
	kdDebug() << "ainc" << endl;
}
;

ColType:
SQL_TYPE
{
	field = new KexiDB::Field();
	field->setType($1);
}
| SQL_TYPE '(' INTEGER_CONST ')'
{
	kdDebug() << "sql + length" << endl;
	field = new KexiDB::Field();
	field->setPrecision($3);
	field->setType($1);
}
| VARCHAR '(' INTEGER_CONST ')'
{
	field = new KexiDB::Field();
	field->setPrecision($3);
	field->setType(KexiDB::Field::Text);
}
|
{
	// SQLITE compatibillity
	field = new KexiDB::Field();
	field->setType(KexiDB::Field::InvalidType);
}
;

SelectStatement:
Select ColViews
{
	kdDebug() << "Select ColViews=" << $2->debugString() << endl;
	
/*
	parser->select()->setBaseTable($4);
	if(parser->select()->unresolvedWildcard() && parser->db())
	{
		//resolve a (pure) wildcard

		TableStruct s = parser->db()->structure($4);

		for(Field *it = s.first(); it; it = s.next())
		{
			parser->select()->addCol(*it);
		}
		parser->select()->setUnresolvedWildcard(false);
	}
*/
}
| Select ColViews Tables
{
//TODO: move this to all SELECT versions
	
	KexiDB::QuerySchema* querySchema = $1;
	$$ = querySchema;
	
	//-------tables list
	KexiDB::NArgExpr *tablesList = $3;
	assert( tablesList ); //&& tablesList->exprClass() == KexiDBExpr_TableList );

	uint columnNum = 0;
/*TODO: use this later if there are columns that use database fields, 
        e.g. "SELECT 1 from table1 t, table2 t") is ok however. */
	//used to collect information about first repeated table name or alias:
//	QDict<char> tableNamesAndTableAliases(997, false);
//	QString repeatedTableNameOrTableAlias;
	//collects positions of tables/aliases with the same names
	QDict< QValueList<int> > repeatedTablesAndAliases(997, false);
	repeatedTablesAndAliases.setAutoDelete(true);
	
	for (int i=0; i<tablesList->args(); i++, columnNum++) {
		BaseExpr *e = tablesList->arg(i);
		VariableExpr* t_e = 0;
		QCString aliasString;
		if (e->exprClass() == KexiDBExpr_SpecialBinary) {
			BinaryExpr* t_with_alias = dynamic_cast<BinaryExpr*>(e);
			assert(t_with_alias);
			assert(t_with_alias->left()->exprClass() == KexiDBExpr_Variable);
			assert(t_with_alias->right()->exprClass() == KexiDBExpr_Variable
				&& (t_with_alias->type()==AS || t_with_alias->type()==0));
			t_e = dynamic_cast<VariableExpr*>(t_with_alias->left());
			aliasString = dynamic_cast<VariableExpr*>(t_with_alias->right())->name.latin1();
		}
		else {
			t_e = dynamic_cast<VariableExpr*>(e);
		}
		assert(t_e);
		QString tname = t_e->name.latin1();
		KexiDB::TableSchema *s = parser->db()->tableSchema(tname);
		if(!s) {
			setError(i18n("Field List Error"), 
				i18n("Table \"%1\" does not exist").arg(tname));
//			yyerror("fieldlisterror");
			return 0;
		}
		QCString tableOrAliasName;
		if (!aliasString.isEmpty()) {
			tableOrAliasName = aliasString;
			kdDebug() << "- add alias for table: " << aliasString << endl;
		} else {
			tableOrAliasName = tname;
		}
		// 1. collect information about first repeated table name or alias
		//    (potential ambiguity)
		QValueList<int> *list = repeatedTablesAndAliases[tableOrAliasName];
		if (list) {
			//another table/alias with the same name
			list->append( i );
			kdDebug() << "- another table/alias with name: " << tableOrAliasName << endl;
		}
		else {
			list = new QValueList<int>();
			list->append( i );
			repeatedTablesAndAliases.insert( tableOrAliasName, list );
			kdDebug() << "- first table/alias with name: " << tableOrAliasName << endl;
		}
/*		if (repeatedTableNameOrTableAlias.isEmpty()) {
			if (tableNamesAndTableAliases[tname])
				repeatedTableNameOrTableAlias=tname;
			else
				tableNamesAndTableAliases.insert(tname, (const char*)1);
		}
		if (!aliasString.isEmpty()) {
			kdDebug() << "- add alias for table: " << aliasString << endl;
//			querySchema->setTableAlias(columnNum, aliasString);
			//2. collect information about first repeated table name or alias
			//   (potential ambiguity)
			if (repeatedTableNameOrTableAlias.isEmpty()) {
				if (tableNamesAndTableAliases[aliasString])
					repeatedTableNameOrTableAlias=aliasString;
				else
					tableNamesAndTableAliases.insert(aliasString, (const char*)1);
			}
		}*/
		kdDebug() << "addTable: " << tname << endl;
		querySchema->addTable( s, aliasString );
	}

	/* set parent table if there's only one */
//	if (parser->select()->tables()->count()==1)
	if (querySchema->tables()->count()==1)
		querySchema->setParentTable(querySchema->tables()->first());

	//-------add fields
	KexiDB::BaseExpr *e;
	columnNum = 0;
	for (KexiDB::BaseExpr::ListIterator it($2->list); (e = it.current()); ++it, columnNum++)
	{
		BaseExpr *columnVariable;
		if (e->exprClass() == KexiDBExpr_SpecialBinary && dynamic_cast<BinaryExpr*>(e)) {
			columnVariable = dynamic_cast<BinaryExpr*>(e)->left();
		}
		else if (e->exprClass() == KexiDBExpr_Variable) {
			columnVariable = e;
		}
		else {
			setError(i18n("Invalid column definition")); //ok?
			return 0;
		}

		if (!addColumn( querySchema, columnVariable, repeatedTablesAndAliases ))
			return 0;
		
		if (e->exprClass() == KexiDBExpr_SpecialBinary && dynamic_cast<BinaryExpr*>(e)
			&& (e->type()==AS || e->type()==0))
		{
			//also add alias
			VariableExpr* aliasVariable =
				dynamic_cast<VariableExpr*>(dynamic_cast<BinaryExpr*>(e)->right());
			if (!aliasVariable) {
				setError(i18n("Invalid column alias definition")); //ok?
				return 0;
			}
			kdDebug() << "ALIAS \"" << aliasVariable->name << "\" set for column " 
				<< columnNum << endl;
			querySchema->setColumnAlias(columnNum, aliasVariable->name.latin1());
		}
	}
	kdDebug() << "Select ColViews=" << $2->debugString() 
		<< " Tables=" << $3->debugString() << endl;
	
	delete $2; //no longer needed
}
| Select Tables
{
	kdDebug() << "Select ColViews Tables" << endl;
}
| Select ColViews WhereClause
{
	kdDebug() << "Select ColViews Conditions" << endl;
}
| Select ColViews Tables WhereClause
{
	kdDebug() << "Select ColViews Tables Conditions" << endl;
}
;

Select:
SELECT
{
	kdDebug() << "SELECT" << endl;
//	parser->createSelect();
//	parser->setOperation(KexiDB::Parser::OP_Select);
	$$ = new QuerySchema();
}
;

WhereClause:
WHERE aExpr
{
	$$ = $2;
}
/*|
{
	$$ = NULL;
}*/
;

aExpr:
aExpr AND aExpr
{
//	kdDebug() << "AND " << $3.debugString() << endl;
	$$ = new KexiDB::BinaryExpr( KexiDBExpr_Logical, $1, AND, $3 );
}
| aExpr OR aExpr
{
//	kdDebug() << "OR " << $3 << endl;
	$$ = new KexiDB::BinaryExpr( KexiDBExpr_Logical, $1, OR, $3 );
}
| NOT aExpr
{
	$$ = new KexiDB::UnaryExpr( NOT, $2 );
//	$$->setName($1->name() + " NOT " + $3->name());
}
| aExpr '+' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '+', $3);
}
| aExpr '-' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '-', $3);
}
| aExpr '/' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '/', $3);
}
| aExpr '*' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '*', $3);
}
| aExpr '%' aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Arithm, $1, '%', $3);
}
| aExpr NOT_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, NOT_EQUAL, $3);
}
| aExpr GREATER_THAN %prec GREATER_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, GREATER_THAN, $3);
}
| aExpr GREATER_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, GREATER_OR_EQUAL, $3);
}
| aExpr LESS_THAN %prec LESS_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, LESS_THAN, $3);
}
| aExpr LESS_OR_EQUAL aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, LESS_OR_EQUAL, $3);
}
| aExpr LIKE aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, LIKE, $3);
}
| aExpr SQL_IN aExpr
{
	$$ = new KexiDB::BinaryExpr(KexiDBExpr_Relational, $1, SQL_IN, $3);
}
| IDENTIFIER '(' aExprList ')'
{
	kdDebug() << "  + function: " << $1 << "(" << $3->debugString() << ")" << endl;
	$$ = new KexiDB::FunctionExpr($1, $3);
}
| IDENTIFIER
{
	$$ = new KexiDB::VariableExpr( QString::fromLatin1($1) );
	
//TODO: simplify this later if that's 'only one field name' expression
	kdDebug() << "  + identifier: " << $1 << endl;
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	$$->setTable(dummy);

//	parser->select()->addField(field);
	requiresTable = true;
}
/*TODO: shall we also support db name? */
| IDENTIFIER '.' IDENTIFIER
{
	$$ = new KexiDB::VariableExpr( QString::fromLatin1($1) + "." + QString::fromLatin1($3) );
	kdDebug() << "  + identifier.identifier: " << $3 << "." << $1 << endl;
//	$$ = new KexiDB::Field();
//	s->setTable($1);
//	$$->setName($3);
	//$$->setTable(parser->db()->tableSchema($1));
//	parser->select()->addField(field);
//??	requiresTable = true;
}
| SQL_NULL
{
	$$ = new KexiDB::ConstExpr( SQL_NULL, QVariant() );
	kdDebug() << "  + NULL" << endl;
//	$$ = new KexiDB::Field();
	//$$->setName(QString::null);
}
| CHARACTER_STRING_LITERAL
{
	$$ = new KexiDB::ConstExpr( CHARACTER_STRING_LITERAL, $1 );
//	$$ = new KexiDB::Field();
//	$$->setName($1);
//	parser->select()->addField(field);
	kdDebug() << "  + constant \"" << $1 << "\"" << endl;
}
| INTEGER_CONST
{
	$$ = new KexiDB::ConstExpr( INTEGER_CONST, $1 );
//	$$ = new KexiDB::Field();
//	$$->setName(QString::number($1));
//	parser->select()->addField(field);
	kdDebug() << "  + int constant: " << $1 << endl;
}
| REAL_CONST
{
	$$ = new KexiDB::ConstExpr( REAL_CONST, QPoint( $1.integer, $1.fractional ) );
	kdDebug() << "  + real constant: " << $1.integer << "." << $1.fractional << endl;
}
| '(' aExpr ')'
{
	kdDebug() << "(expr)" << endl;
	$$ = $2;
}
;

aExprList:
aExprList ',' aExpr
{
	$1->add( $3 );
	$$ = $1;
}
|/* EMPTY */
{
	$$ = new KexiDB::NArgExpr(0, 0/*unknown*/);
}
;

Tables:
FROM FlatTableList
{
	$$ = $2;
}
/*
| Tables LEFT JOIN IDENTIFIER SQL_ON ColExpression
{
	kdDebug() << "LEFT JOIN: '" << $4 << "' ON " << $6 << endl;
	addTable($4);
}
| Tables LEFT OUTER JOIN IDENTIFIER SQL_ON ColExpression
{
	kdDebug() << "LEFT OUTER JOIN: '" << $5 << "' ON " << $7 << endl;
	addTable($5);
}
| Tables INNER JOIN IDENTIFIER SQL_ON ColExpression
{
	kdDebug() << "INNER JOIN: '" << $4 << "' ON " << $6 << endl;
	addTable($4);
}
| Tables RIGHT JOIN IDENTIFIER SQL_ON ColExpression
{
	kdDebug() << "RIGHT JOIN: '" << $4 << "' ON " << $6 << endl;
	addTable($4);
}
| Tables RIGHT OUTER JOIN IDENTIFIER SQL_ON ColExpression
{
	kdDebug() << "RIGHT OUTER JOIN: '" << $5 << "' ON " << $7 << endl;
	addTable($5);
}*/
;

/*
FlatTableList:
aFlatTableList
{
	$$
}
;*/

FlatTableList:
FlatTableList ',' FlatTable
{
	$$ = $1;
	$$->add($3);
}
|FlatTable
{
	$$ = new KexiDB::NArgExpr(KexiDBExpr_TableList, IDENTIFIER); //ok?
	$$->add($1);
}
;

FlatTable:
IDENTIFIER
{
	kdDebug() << "FROM: '" << $1 << "'" << endl;

//	KexiDB::TableSchema *schema = parser->db()->tableSchema($1);
//	parser->select()->setParentTable(schema);
//	parser->select()->addTable(schema);
//	requiresTable = false;
	
//addTable($1);

	$$ = new VariableExpr(QString::fromLatin1($1));

	/*
//TODO: this isn't ok for more tables:
	KexiDB::Field::ListIterator it = parser->select()->fieldsIterator();
	for(KexiDB::Field *item; (item = it.current()); ++it)
	{
		if(item->table() == dummy)
		{
			item->setTable(schema);
		}

		if(item->table() && !item->isQueryAsterisk())
		{
			KexiDB::Field *f = item->table()->field(item->name());
			if(!f)
			{
				KexiDB::ParserError err(i18n("Field List Error"), i18n("Unknown column '%1' in table '%2'").arg(item->name()).arg(schema->name()), ctoken, current);
				parser->setError(err);
				yyerror("fieldlisterror");
			}	
		}
	}*/
}
| IDENTIFIER IDENTIFIER
{
	//table + alias
	$$ = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, 
		new KexiDB::VariableExpr(QString::fromLatin1($1)), 0,
		new KexiDB::VariableExpr(QString::fromLatin1($2))
	);
}
| IDENTIFIER AS IDENTIFIER
{
	//table + alias
	$$ = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary,
		new KexiDB::VariableExpr(QString::fromLatin1($1)), AS,
		new KexiDB::VariableExpr(QString::fromLatin1($3))
	);
}
;



ColViews:
ColViews ',' ColItem
{
	$$ = $1;
	$$->add( $3 );
	kdDebug() << "ColViews: ColViews , ColItem" << endl;
}
|ColItem
{
	$$ = new KexiDB::NArgExpr(0,0);
	$$->add( $1 );
	kdDebug() << "ColViews: ColItem" << endl;
}
;

ColItem:
ColExpression
{
//	$$ = new KexiDB::Field();
//	dummy->addField($$);
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	$$ = $1;
	kdDebug() << " added column expr: '" << $1->debugString() << "'" << endl;
}
| ColWildCard
{
	$$ = $1;
	kdDebug() << " added column wildcard: '" << $1->debugString() << "'" << endl;
}
| ColExpression AS IDENTIFIER
{
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	$$ = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, $1, AS,
		new KexiDB::VariableExpr(QString::fromLatin1($3))
//		new KexiDB::ConstExpr(IDENTIFIER, QString::fromLocal8Bit($3))
	);
	kdDebug() << " added column expr: " << $$->debugString() << endl;
}
| ColExpression IDENTIFIER
{
//	$$ = new KexiDB::Field();
//	$$->setExpression( $1 );
//	parser->select()->addField($$);
	$$ = new KexiDB::BinaryExpr(
		KexiDBExpr_SpecialBinary, $1, 0, 
		new KexiDB::VariableExpr(QString::fromLatin1($2))
//		new KexiDB::ConstExpr(IDENTIFIER, QString::fromLocal8Bit($2))
	);
	kdDebug() << " added column expr: " << $$->debugString() << endl;
}
;

ColExpression:
aExpr
{
	$$ = $1;
}
/*
| SUM '(' ColExpression ')'
{
//	$$ = new KexiDB::AggregationExpr( SUM,  );
//TODO
//	$$->setName("SUM(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| SQL_MIN '(' ColExpression ')'
{
	$$ = $3;
//TODO
//	$$->setName("MIN(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| SQL_MAX '(' ColExpression ')'
{
	$$ = $3;
//TODO
//	$$->setName("MAX(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}
| AVG '(' ColExpression ')'
{
	$$ = $3;
//TODO
//	$$->setName("AVG(" + $3->name() + ")");
//wait	$$->containsGroupingAggregate(true);
//wait	parser->select()->grouped(true);
}*/
//?
| DISTINCT '(' ColExpression ')' 
{
	$$ = $3;
//TODO
//	$$->setName("DISTINCT(" + $3->name() + ")");
}
;

ColWildCard:
'*'
{
	$$ = new KexiDB::VariableExpr("*");
	kdDebug() << "all columns" << endl;

//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), dummy);
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
}
| IDENTIFIER_DOT_ASTERISK
{
	$$ = new KexiDB::VariableExpr(QString::fromLatin1($1));
	kdDebug() << "  + all columns from " << $1 << endl;
//	KexiDB::QueryAsterisk *ast = new KexiDB::QueryAsterisk(parser->select(), parser->db()->tableSchema($1));
//	parser->select()->addAsterisk(ast);
//	requiresTable = true;
}
/*| ERROR_DIGIT_BEFORE_IDENTIFIER
{
	$$ = new KexiDB::VariableExpr($1);
	kdDebug() << "  Invalid identifier! " << $1 << endl;
	setError(i18n("Invalid identifier \"%1\"").arg($1));
}*/
;

%%

