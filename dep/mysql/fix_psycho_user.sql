-- ===========================================================================
-- Psycho_Core - Reset DB user/password to match conf.dist
-- Run this in the MariaDB console as root.
--   mysql_console.bat   (from server root), enter root password, then:
--       source fix_psycho_user.sql
--   OR paste the commands below directly.
-- ===========================================================================

-- Drop any wrong-case users that may exist.
DROP USER IF EXISTS 'Psycho'@'localhost';
DROP USER IF EXISTS 'Psycho'@'127.0.0.1';

-- Recreate the lowercase 'psycho' user with password 'psycho'.
-- This matches worldserver.conf / bnetserver.conf: psycho / psycho
CREATE USER IF NOT EXISTS 'psycho'@'localhost' IDENTIFIED BY 'psycho';
CREATE USER IF NOT EXISTS 'psycho'@'127.0.0.1' IDENTIFIED BY 'psycho';

-- Grant full access to all Psycho_Core databases.
GRANT ALL PRIVILEGES ON psycho_auth.*       TO 'psycho'@'localhost';
GRANT ALL PRIVILEGES ON psycho_characters.* TO 'psycho'@'localhost';
GRANT ALL PRIVILEGES ON psycho_world.*      TO 'psycho'@'localhost';
GRANT ALL PRIVILEGES ON psycho_hotfixes.*   TO 'psycho'@'localhost';

GRANT ALL PRIVILEGES ON psycho_auth.*       TO 'psycho'@'127.0.0.1';
GRANT ALL PRIVILEGES ON psycho_characters.* TO 'psycho'@'127.0.0.1';
GRANT ALL PRIVILEGES ON psycho_world.*      TO 'psycho'@'127.0.0.1';
GRANT ALL PRIVILEGES ON psycho_hotfixes.*   TO 'psycho'@'127.0.0.1';

FLUSH PRIVILEGES;

SELECT User, Host FROM mysql.user WHERE User LIKE 'psycho%' OR User = 'Psycho';
SHOW DATABASES;
