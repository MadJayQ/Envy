namespace LoginClient
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.tbUsr = new System.Windows.Forms.TextBox();
            this.tbPswd = new System.Windows.Forms.TextBox();
            this.btnLogin = new System.Windows.Forms.Button();
            this.lblUsr = new System.Windows.Forms.Label();
            this.lblPswd = new System.Windows.Forms.Label();
            this.tbHost = new System.Windows.Forms.TextBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // tbUsr
            // 
            this.tbUsr.Location = new System.Drawing.Point(59, 136);
            this.tbUsr.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tbUsr.Name = "tbUsr";
            this.tbUsr.Size = new System.Drawing.Size(142, 20);
            this.tbUsr.TabIndex = 0;
            this.tbUsr.TextChanged += new System.EventHandler(this.tbUsr_TextChanged);
            // 
            // tbPswd
            // 
            this.tbPswd.Location = new System.Drawing.Point(58, 186);
            this.tbPswd.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tbPswd.Name = "tbPswd";
            this.tbPswd.Size = new System.Drawing.Size(142, 20);
            this.tbPswd.TabIndex = 1;
            this.tbPswd.UseSystemPasswordChar = true;
            // 
            // btnLogin
            // 
            this.btnLogin.Location = new System.Drawing.Point(60, 222);
            this.btnLogin.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnLogin.Name = "btnLogin";
            this.btnLogin.Size = new System.Drawing.Size(141, 19);
            this.btnLogin.TabIndex = 2;
            this.btnLogin.Text = "Login";
            this.btnLogin.UseVisualStyleBackColor = true;
            this.btnLogin.Click += new System.EventHandler(this.btnLogin_Click);
            // 
            // lblUsr
            // 
            this.lblUsr.AutoSize = true;
            this.lblUsr.Location = new System.Drawing.Point(99, 121);
            this.lblUsr.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblUsr.Name = "lblUsr";
            this.lblUsr.Size = new System.Drawing.Size(55, 13);
            this.lblUsr.TabIndex = 3;
            this.lblUsr.Text = "Username";
            // 
            // lblPswd
            // 
            this.lblPswd.AutoSize = true;
            this.lblPswd.Location = new System.Drawing.Point(101, 171);
            this.lblPswd.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblPswd.Name = "lblPswd";
            this.lblPswd.Size = new System.Drawing.Size(53, 13);
            this.lblPswd.TabIndex = 4;
            this.lblPswd.Text = "Password";
            // 
            // tbHost
            // 
            this.tbHost.Location = new System.Drawing.Point(93, 255);
            this.tbHost.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.tbHost.Name = "tbHost";
            this.tbHost.Size = new System.Drawing.Size(70, 20);
            this.tbHost.TabIndex = 5;
            this.tbHost.Text = "127.0.0.1";
            // 
            // pictureBox1
            // 
            this.pictureBox1.InitialImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.InitialImage")));
            this.pictureBox1.Location = new System.Drawing.Point(12, 12);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(235, 103);
            this.pictureBox1.TabIndex = 6;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(263, 325);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.tbHost);
            this.Controls.Add(this.lblPswd);
            this.Controls.Add(this.lblUsr);
            this.Controls.Add(this.btnLogin);
            this.Controls.Add(this.tbPswd);
            this.Controls.Add(this.tbUsr);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "Form1";
            this.Text = "Form1";
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
    }
}

