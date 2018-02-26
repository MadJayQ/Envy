package envyfileserver.login;

import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import java.security.SecureRandom;
import java.sql.*;
import java.util.Base64;
import java.util.logging.Level;
import java.util.logging.Logger;

public class DBManager {

    /**
     * *************************************************
     * Create Database *************************************************
     */
    public static void createSQLiteDB(String filename) {

        String URL = "jdbc:sqlite:" + filename;
        try {
            Connection conn = DriverManager.getConnection(URL);
            if (conn != null) {
                // DatabaseMetaData meta = conn.getMetaData();
                conn.close();
            }

        } catch (SQLException e) {
            System.out.println("Error creating the database.\n" + e.getMessage());
            System.exit(1);
        }
    }

    public static void createSQLLiteRolesTable(String filename) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "";

        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            stmt.close();
            conn.close();
        } catch (SQLException e) {
            System.err.println(e.getClass().getName());
            System.exit(1);
        }
    }

    /**
     * *************************************************
     * Create Users table in the database
     * *************************************************
     */
    public static void createSQLiteLoginTable(String filename) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "CREATE TABLE IF NOT EXISTS users ( userID INT NOT NULL PRIMARY KEY, first_name VARCHAR(32) NOT NULL, email VARCHAR(32) NOT NULL, username VARCHAR(32) NOT NULL, salt VARCHAR(128) NOT NULL, passhash VARCHAR(128) NOT NULL)";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            stmt.executeUpdate(sqlcommand);
            stmt.close();
            conn.close();
        } catch (SQLException e) {
            System.err.println(e.getClass().getName());
            System.out.println("Error opening the database: " + e.getMessage());
            System.exit(1);
        }
    }

    public static void createSQLiteRolesTable(String filename) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "CREATE TABLE IF NOT EXISTS roles ("
                + "roleID INT NOT NULL PRIMARY KEY,"
                + "roleName VARCHAR(20) NOT NULL);";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            stmt.executeUpdate(sqlcommand);
            stmt.close();
            conn.close();
        } catch (SQLException ex) {
            System.err.println(ex.getClass().getName());
            System.out.println("Error opening the database: " + ex.getMessage());
            System.exit(1);
        }
    }

    public static void createSQLiteUserRolesTable(String filename) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "CREATE TABLE IF NOT EXISTS userroles ("
                + "userID INT NOT NULL,"
                + "roleID INT NOT NULL,"
                + "FOREIGN KEY (userID) REFERENCES users (userID),"
                + "FOREIGN KEY (roleID) REFERENCES roles (roleID));";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            stmt.executeUpdate(sqlcommand);
            stmt.close();
            conn.close();
        } catch (SQLException ex) {
            System.err.println(ex.getClass().getName());
            System.out.println("Error opening the database: " + ex.getMessage());
            System.exit(1);
        }
    }

    /*
            Our query and insert statements were not aligned, we need to take into account the userID
     */
    public static void createSQLiteLoginUser(String filename, String firstName, String lastName, String username,
            String password, int uid) {
        String URL = "jdbc:sqlite:" + filename;
        byte[] salt = new byte[16];
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "";
        try {
            SecureRandom sr = SecureRandom.getInstance("SHA1PRNG");
            sr.nextBytes(salt);
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            PBEKeySpec spec = new PBEKeySpec(password.toCharArray(), salt, 65536, 128);
            SecretKeyFactory skey = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA1");
            byte[] hash = skey.generateSecret(spec).getEncoded();
            sqlcommand = "INSERT OR IGNORE into users values('" + uid + "','" + firstName + "','" + lastName + "','" + username + "','"
                    + Base64.getEncoder().encodeToString(salt) + "', '" + Base64.getEncoder().encodeToString(hash)
                    + "');";
            stmt.executeUpdate(sqlcommand);
            stmt.close();
            conn.close();

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error openiang the database.");
            System.exit(1);
        }
    }

    public static void createSQLiteUserRole(String filename, int userID, int roleID) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "INSERT or IGNORE into userroles values('" + userID + "','" + roleID + "');";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            stmt.executeUpdate(sqlcommand);
            stmt.close();
            conn.close();

        } catch (SQLException ex) {
            ex.printStackTrace();
            System.out.println("Error openiang the database.");
            System.exit(1);
        }
    }

    public static String getSQLiteUserRole(String filename, String username) {
        String URL = "jdbc:sqlite:" + filename;
        String role = "";
        Connection conn = null;
        Statement stmt = null;
        ResultSet rst = null;
        String sqlcommand = "select userID from users where USERNAME='" + username + "';";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            rst = stmt.executeQuery(sqlcommand);
            while (rst.next()) {
                String userID = rst.getString("userID");
                String roleID = "";
                if (userID.length() > 0) {
                    sqlcommand = "select roleID from userroles where userID='" + userID + "';";
                    rst = stmt.executeQuery(sqlcommand);
                    while (rst.next()) {
                        roleID = rst.getString("roleID");
                        return roleID;
                    }
                }
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
            System.out.println("Error openiang the database.");
            System.exit(1);
        }
        return role;
    }

    public static void createSQLiteRole(String filename, String roleName, int roleID) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        String sqlcommand = "INSERT or IGNORE into roles values('" + roleID + "','" + roleName + "');";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            stmt.executeUpdate(sqlcommand);
            stmt.close();
            conn.close();

        } catch (SQLException ex) {
            ex.printStackTrace();
            System.out.println("Error openiang the database.");
            System.exit(1);
        }
    }

    public static int getNextAvailableID(String filename) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        ResultSet rst = null;
        int ret = 0;
        String sqlcommand = "SELECT t1.userID + 1 "
                + "FROM users t1 "
                + "WHERE NOT EXISTS ("
                + "    SELECT *"
                + "    FROM users t2"
                + "    WHERE t2.userID = t1.userID + 1"
                + ") "
                + "LIMIT 1";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            rst = stmt.executeQuery(sqlcommand);
            while (rst.next()) {
                ret = rst.getInt(1);
            }
            stmt.close();
            conn.close();
        } catch (Exception e) {
            System.out.println("Error opening the database.");
            ret = -1;
        }

        return ret;
    }

    public static boolean checkSQLiteLoginUserUnique(String filename, String username) {
        String URL = "jdbc:sqlite:" + filename;
        Connection conn = null;
        Statement stmt = null;
        ResultSet rst = null;
        String sqlcommand = "";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            sqlcommand = "select * from USERS where USERNAME='" + username + "';";
            rst = stmt.executeQuery(sqlcommand);
            while (rst.next()) {
                rst.close();
                stmt.close();
                conn.close();
                return false;
            }
            stmt.close();
            conn.close();

            return true;

        } catch (Exception e) {
            System.out.println("Error opening the database.");
            System.exit(1);
        }
        return false;
    }

    /**
     * *************************************************
     * Authentication *************************************************
     */
    public static String[] checkSQLiteLoginUser(String filename, String username, String password) {
        // System.out.println(username+" ---- "+password);
        String URL = "jdbc:sqlite:" + filename;
        byte[] salt = new byte[16];
        Connection conn = null;
        Statement stmt = null;
        ResultSet rst = null;
        String sqlcommand = "";
        try {
            conn = DriverManager.getConnection(URL);
            stmt = conn.createStatement();
            sqlcommand = "select * from USERS where USERNAME='" + username + "';";
            rst = stmt.executeQuery(sqlcommand);
            while (rst.next()) {
                salt = Base64.getDecoder().decode(rst.getString("SALT"));
                PBEKeySpec spec = new PBEKeySpec(password.toCharArray(), salt, 65536, 128);
                SecretKeyFactory skey = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA1");
                byte[] hash = skey.generateSecret(spec).getEncoded();
                if ((Base64.getEncoder().encodeToString(hash)).equals(rst.getString("PASSHASH"))) {
                    String[] myReturnString = new String[2];
                    myReturnString[0] = rst.getString("FIRST_NAME");
                    //myReturnString[1] = rst.getString("LAST_NAME");
                    rst.close();
                    stmt.close();
                    conn.close();
                    return myReturnString;
                }
            }
            stmt.close();
            conn.close();

        } catch (Exception e) {
            System.out.println("Error opening the database.");
            System.exit(1);
        }
        return null;
    }

}
