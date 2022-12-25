using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Binarisation
{
	/// <summary>
	/// Summary description for RichTextWindow.
	/// </summary>
	public class RichTextWindow : System.Windows.Forms.Form
	{
        private System.Windows.Forms.RichTextBox richTextBox;
        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem menuItemClear;
        private System.Windows.Forms.MenuItem menuItemHide;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public RichTextWindow()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.richTextBox = new System.Windows.Forms.RichTextBox();
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItemClear = new System.Windows.Forms.MenuItem();
			this.menuItemHide = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// richTextBox
			// 
			this.richTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.richTextBox.Location = new System.Drawing.Point(0, 0);
			this.richTextBox.Name = "richTextBox";
			this.richTextBox.ReadOnly = true;
			this.richTextBox.Size = new System.Drawing.Size(596, 360);
			this.richTextBox.TabIndex = 0;
			this.richTextBox.Text = "";
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItemClear,
																					  this.menuItemHide});
			// 
			// menuItemClear
			// 
			this.menuItemClear.Index = 0;
			this.menuItemClear.Text = "&Clear";
			this.menuItemClear.Click += new System.EventHandler(this.menuItemClear_Click);
			// 
			// menuItemHide
			// 
			this.menuItemHide.Index = 1;
			this.menuItemHide.Text = "&Hide";
			this.menuItemHide.Click += new System.EventHandler(this.menuItemHide_Click);
			// 
			// RichTextWindow
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(592, 373);
			this.Controls.Add(this.richTextBox);
			this.Menu = this.mainMenu1;
			this.MinimumSize = new System.Drawing.Size(600, 400);
			this.Name = "RichTextWindow";
			this.Text = "RichTextWindow";
			this.TopMost = true;
			this.Closing += new System.ComponentModel.CancelEventHandler(this.RichTextWindow_Closing);
			this.Load += new System.EventHandler(this.RichTextWindow_Load);
			this.ResumeLayout(false);

		}
		#endregion

        public void Clear()
        {
            richTextBox.Clear();
        }

        public void WriteLine(string text)
        {
            richTextBox.AppendText(text + Environment.NewLine);
        }

        private void menuItemClear_Click(object sender, System.EventArgs e)
        {
            Clear();
        }

        private void menuItemHide_Click(object sender, System.EventArgs e)
        {
            Hide();
        }

        private void RichTextWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // Prevent closing form... just hide it
            Hide();
            e.Cancel = true;
        }

		private void RichTextWindow_Load(object sender, System.EventArgs e)
		{
		
		}

	}
}
