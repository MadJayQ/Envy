package envyfileserver;

import envyfileserver.login.DBManager;
import envyfileserver.login.loginDialog;
import envyfileserver.schema.User;

public abstract class LoginServerCore {
    
    public final static String dbFileName = "./data.sqlite";
    public final static String[] roles = {"Admin", "Moderator", "User"};
    
    
    public LoginServerCore() {
        
    }
    
    public void run() {
        try {
            init();
        }
        finally {
            //Cleanup
            exit();
        }
    }
    
    public void exit() {
        
    }
    
    /*
    Initialize our sqlite database
    */
    public void init() {
        DBManager.createSQLiteDB(dbFileName);
        DBManager.createSQLiteLoginTable(dbFileName);
        DBManager.createSQLiteRolesTable(dbFileName);
        DBManager.createSQLiteUserRolesTable(dbFileName);
        
        for(int i = 1; i <= roles.length; i++) {
            DBManager.createSQLiteRole(dbFileName, roles[i-1], i);
        }
        
        //Add test to modetator status
        /*
        DBManager.createSQLiteLoginUser(dbFileName, "Jake", "Thornton", "madjayq", "Jakenathan47%", 1);
        DBManager.createSQLiteLoginUser(dbFileName, "Cameron", "", "fakie", "test123", 2);
        DBManager.createSQLiteUserRole(dbFileName, 1, 1);
        DBManager.createSQLiteUserRole(dbFileName, 2, 2);
        DBManager.createSQLiteUserRole(dbFileName, 3, User.Role.ROLE_USER.getValue());
        DBManager.createSQLiteUserRole(dbFileName, 4, User.Role.ROLE_USER.getValue());
        */
        
        User admin = new User("Jake", "madjayq", "Jakenathan47%", "MadJayQ@envycheat.cc", User.Role.ROLE_ADMIN);
        User cam = new User("Cameron", "cameron", "temporaryidk", "fakietko@gmail.com", User.Role.ROLE_MODERATOR);
        User dj = new User("DJ", "zandaron", "test123", "donaldallen444@gmail.com", User.Role.ROLE_USER);
        admin.insertToDatabase(dbFileName);
        cam.insertToDatabase(dbFileName);
        dj.insertToDatabase(dbFileName);
        
        /*
        String[] info = loginDialog.loginDialogOpen("Login");
        
        String[] userInfo = DBManager.checkSQLiteLoginUser(dbFileName, info[0], info[1]);
        if(userInfo != null) {
            String role = DBManager.getSQLiteUserRole(dbFileName, info[0]);
            System.out.println("Username: " + userInfo[0] + " Role: " + role);
        } else {
            System.out.println("Username: " + info[0] + " not found!");
        }
*/
    }
}
