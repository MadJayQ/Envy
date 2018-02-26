namespace LoginClient
{
    partial class LoginClient
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LoginClient));
            this.tbUsr = new System.Windows.Forms.TextBox();
            this.tbPswd = new System.Windows.Forms.TextBox();
            this.btnLogin = new System.Windows.Forms.Button();
            this.lblUsr = new System.Windows.Forms.Label();
            this.lblPswd = new System.Windows.Forms.Label();
            this.tbHost = new System.Windows.Forms.TextBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.lblStatus = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // tbUsr
            // 
            this.tbUsr.Location = new System.Drawing.Point(79, 167);
            this.tbUsr.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tbUsr.Name = "tbUsr";
            this.tbUsr.Size = new System.Drawing.Size(188, 22);
            this.tbUsr.TabIndex = 0;
            this.tbUsr.TextChanged += new System.EventHandler(this.tbUsr_TextChanged);
            // 
            // tbPswd
            // 
            this.tbPswd.Location = new System.Drawing.Point(77, 229);
            this.tbPswd.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tbPswd.Name = "tbPswd";
            this.tbPswd.Size = new System.Drawing.Size(188, 22);
            this.tbPswd.TabIndex = 1;
            this.tbPswd.UseSystemPasswordChar = true;
            // 
            // btnLogin
            // 
            this.btnLogin.Location = new System.Drawing.Point(80, 273);
            this.btnLogin.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnLogin.Name = "btnLogin";
            this.btnLogin.Size = new System.Drawing.Size(188, 23);
            this.btnLogin.TabIndex = 2;
            this.btnLogin.Text = "Login";
            this.btnLogin.UseVisualStyleBackColor = true;
            this.btnLogin.Click += new System.EventHandler(this.btnLogin_Click);
            // 
            // lblUsr
            // 
            this.lblUsr.AutoSize = true;
            this.lblUsr.Location = new System.Drawing.Point(132, 149);
            this.lblUsr.Name = "lblUsr";
            this.lblUsr.Size = new System.Drawing.Size(73, 17);
            this.lblUsr.TabIndex = 3;
            this.lblUsr.Text = "Username";
            // 
            // lblPswd
            // 
            this.lblPswd.AutoSize = true;
            this.lblPswd.Location = new System.Drawing.Point(135, 210);
            this.lblPswd.Name = "lblPswd";
            this.lblPswd.Size = new System.Drawing.Size(69, 17);
            this.lblPswd.TabIndex = 4;
            this.lblPswd.Text = "Password";
            // 
            // tbHost
            // 
            this.tbHost.Location = new System.Drawing.Point(124, 314);
            this.tbHost.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tbHost.Name = "tbHost";
            this.tbHost.Size = new System.Drawing.Size(92, 22);
            this.tbHost.TabIndex = 5;
            this.tbHost.Text = "127.0.0.1";
            // 
            // pictureBox1
            // 
            this.pictureBox1.InitialImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.InitialImage")));
            this.pictureBox1.Location = new System.Drawing.Point(16, 15);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(313, 127);
            this.pictureBox1.TabIndex = 6;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.ForeColor = System.Drawing.Color.Lime;
            this.lblStatus.Location = new System.Drawing.Point(148, 352);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(54, 17);
            this.lblStatus.TabIndex = 7;
            this.lblStatus.Text = "Sucess";
            this.lblStatus.Visible = false;
            // 
            // LoginClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.ClientSize = new System.Drawing.Size(351, 400);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.tbHost);
            this.Controls.Add(this.lblPswd);
            this.Controls.Add(this.lblUsr);
            this.Controls.Add(this.btnLogin);
            this.Controls.Add(this.tbPswd);
            this.Controls.Add(this.tbUsr);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "LoginClient";
            this.Text = "Envy Login Client";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbUsr;
        private System.Windows.Forms.TextBox tbPswd;
        private System.Windows.Forms.Button btnLogin;
        private System.Windows.Forms.Label lblUsr;
        private System.Windows.Forms.Label lblPswd;
        private System.Windows.Forms.TextBox tbHost;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label lblStatus;
    }
}

