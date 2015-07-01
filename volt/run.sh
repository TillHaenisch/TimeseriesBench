bin/voltdb create > volt.log &
bin/sqlcmd < ddl.sql
java volt_asyncbm
