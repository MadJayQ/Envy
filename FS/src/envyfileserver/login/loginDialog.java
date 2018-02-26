/******************************************************************************
 * Title: 				Login GUI
 * Author: 				Youssif Al-Nashif
 * Email: 				yalnashif@floridapoly.edu
 * Package: 				login
 * Version: 				0.0.1a
 * Creation Date: 		Oct. 15, 2017 @ 22:12
 * Modification Date: 	Oct. 15, 2017 @ 22:12
 * Description: 			Simple login GUI
 ******************************************************************************/

package envyfileserver.login;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.*;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JPasswordField;
import javax.swing.JComboBox;

public class loginDialog extends JDialog {

	private static final long serialVersionUID = 1L;

	private final JPanel contentPanel = new JPanel();

	public String username;
	public String password;
	public String role;
	private JTextField usernameText;
	private JPasswordField passwordText;
	private JLabel roleLabel;
	private JComboBox<String> comboBox1;
	private JButton okButton;

	/**
	 * Create the dialog.
	 */
	public loginDialog() {
		setBounds(100, 100, 450, 300);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(new GridLayout(3, 0, 0, 0));
		setModal(true);
		getRootPane().setDefaultButton(okButton);

		{
			JLabel usernameLabel = new JLabel("Username:");
			contentPanel.add(usernameLabel);
		}
		{
			usernameText = new JTextField();
			contentPanel.add(usernameText);
			usernameText.setColumns(10);
		}
		{
			JLabel passwordLabel = new JLabel("Password:");
			contentPanel.add(passwordLabel);
		}
		{
			passwordText = new JPasswordField();
			contentPanel.add(passwordText);
		}
		{
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
			getContentPane().add(buttonPane, BorderLayout.SOUTH);
			{
				okButton = new JButton("OK");
				okButton.setActionCommand("OK");
				buttonPane.add(okButton);
				getRootPane().setDefaultButton(okButton);
				okButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						onOK();
					}
				});
			}
			{
				JButton cancelButton = new JButton("Cancel");
				cancelButton.setActionCommand("Cancel");
				buttonPane.add(cancelButton);

				cancelButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						onCancel();
					}
				});
			}

		}
	}

	private void onOK() {
		// add your code here
		username = usernameText.getText();
		password = String.valueOf(passwordText.getPassword());
		dispose();
	}

	private void onCancel() {
		// add your code here if necessary
		dispose();
	}

	static public String[] loginDialogOpen(String title) {
		String[] myReturnString;
		loginDialog dialog = new loginDialog();
		dialog.pack();
		Toolkit toolkit = Toolkit.getDefaultToolkit();
		Dimension screenSize = toolkit.getScreenSize();
		int x = (screenSize.width - dialog.getWidth()) / 2;
		int y = (screenSize.height - dialog.getHeight()) / 2;
		dialog.setLocation(x, y);
		dialog.setResizable(false);
		dialog.setTitle(title);
		dialog.setVisible(true);
		if (dialog.username != null) {
			if (dialog.role != null) {
				myReturnString = new String[3];
				myReturnString[0] = dialog.username;
				myReturnString[1] = dialog.password;
			} else {
				myReturnString = new String[2];
				myReturnString[0] = dialog.username;
				myReturnString[1] = dialog.password;
			}
		} else
			myReturnString = null;
		return myReturnString;
	}

}
