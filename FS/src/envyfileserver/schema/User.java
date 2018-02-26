/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package envyfileserver.schema;

import envyfileserver.LoginServerCore;
import envyfileserver.login.DBManager;

/**
 *
 * @author jithornton47
 */
public class User {

    public static enum Role {
        ROLE_ADMIN(1),
        ROLE_MODERATOR(2),
        ROLE_USER(3);

        private int val;

        private Role(int val) {
            this.val = val;
        }

        public int getValue() {
            return this.val;
        }
        
        public static Role fromID(int id) {
            return Role.values()[id - 1];
        }
        
        @Override
        public String toString() {
            return LoginServerCore.roles[this.val - 1];
        }
    };

    private Role userRole;
    private String name;
    private String user;
    private String pass;
    private String email;
    private int userID;

    public User(String name, String user, String pass, String email, Role role) {
        this.name = name;
        this.user = user;
        this.pass = pass;
        this.email = email;
        this.userRole = role;
    }

    public User(String user, String pass) {
        this.user = user;
        this.pass = pass;
    }

    public boolean authenticate(String db) {
        String[] userInfo = DBManager.checkSQLiteLoginUser(db, user, pass);
        if (userInfo != null) {
            this.userRole = Role.fromID(Integer.parseInt(DBManager.getSQLiteUserRole(db, user)));
            return true;
        } else {
            return false;
        }
    }

    public String handle() {
        return this.user;
    }

    public String role() {
        return LoginServerCore.roles[this.userRole.getValue() - 1];
    }

    public void insertToDatabase(String db) {
        this.userID = DBManager.getNextAvailableID(db);
        if (this.userID == -1) {
            System.out.println("ERROR INVALID IDX");
            return;
        }
        if (DBManager.checkSQLiteLoginUserUnique(db, user)) {
            DBManager.createSQLiteLoginUser(db, name, email, user, pass, userID);
            DBManager.createSQLiteUserRole(db, userID, userRole.getValue());
        }
    }
}
