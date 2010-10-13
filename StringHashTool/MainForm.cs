using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Gibbed.SaintsRow2.Helpers;

namespace StringHashTool
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void InputTextBox_TextChanged(object sender, EventArgs e)
        {
            CrcVolitionTextBox.Text = String.Format("{0:X8}", StringHelpers.CrcVolition(InputTextBox.Text));
            HashVolitionTextBox.Text = String.Format("{0:X8}", StringHelpers.HashVolition(InputTextBox.Text));
        }
    }
}
