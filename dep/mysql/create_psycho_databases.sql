-- ===========================================================================
-- Psycho_Core portable MariaDB database bootstrap
-- Creates Psycho_Core databases and the psycho DB user.
-- Default DB login for worldserver.conf/bnetserver.conf: psycho / psycho
-- ===========================================================================

CREATE DATABASE IF NOT EXISTS psycho_auth       DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE DATABASE IF NOT EXISTS psycho_characters DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE DATABASE IF NOT EXISTS psycho_world      DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE DATABASE IF NOT EXISTS psycho_hotfixes   DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

CREATE USER IF NOT EXISTS 'psycho'@'localhost' IDENTIFIED BY 'psycho';
CREATE USER IF NOT EXISTS 'psycho'@'127.0.0.1' IDENTIFIED BY 'psycho';

GRANT ALL PRIVILEGES ON psycho_auth.*       TO 'psycho'@'localhost';
GRANT ALL PRIVILEGES ON psycho_characters.* TO 'psycho'@'localhost';
GRANT ALL PRIVILEGES ON psycho_world.*      TO 'psycho'@'localhost';
GRANT ALL PRIVILEGES ON psycho_hotfixes.*   TO 'psycho'@'localhost';

GRANT ALL PRIVILEGES ON psycho_auth.*       TO 'psycho'@'127.0.0.1';
GRANT ALL PRIVILEGES ON psycho_characters.* TO 'psycho'@'127.0.0.1';
GRANT ALL PRIVILEGES ON psycho_world.*      TO 'psycho'@'127.0.0.1';
GRANT ALL PRIVILEGES ON psycho_hotfixes.*   TO 'psycho'@'127.0.0.1';

FLUSH PRIVILEGES;

SHOW DATABASES;
SELECT User, Host FROM mysql.user WHERE User='psycho';
