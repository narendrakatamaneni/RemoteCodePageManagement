/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - Code Publisher Client                      //
// ver 1.2                                                         //
// Narendra Katamaneni, CSE687 - Object Oriented Design            //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines one class MainWindow that provides tabs:
 * - Find Libs: Navigate through local directory to find files for testing
 *   - Shows local directories and files
 *   - Can navigate by double clicking directories
 * 
 * Required Files:
 * ---------------
 * MainWindow.xaml, MainWindow.xaml.cs
 * SelectionWindow.xaml, SelectionWindow.xaml.cs
 * 
 * Maintenance History:
 * -------------------
 * ver 1.0 : 28 Mar 2019
 * - first release
 * 
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using Path = System.IO.Path;

namespace ClientGui
{
    ///////////////////////////////////////////////////////////////////
    // MainWindow class
    // - Provides navigation and selection to find libraries to test.
    // - Creates a popup window to handle selections.

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        private string WDirectory { get; set; }
        private string patterns { get; set; }
        private string regex { get; set; }


        public MainWindow()
        {
            InitializeComponent();
        }


        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            WDirectory = Directory.GetCurrentDirectory();
            WDirectory = getAncestorPath(3, WDirectory);
            txtPath.Text = WDirectory;
            patterns = "*.h *.cpp";
            regex = "[A-B](.*)";
            txtPatterns.Text = patterns;
            txtRegex.Text = regex;
            LoadNavTab(WDirectory);

        }
        void LoadNavTab(string path)
        {
            lstFiles.Items.Clear();
            //CurrPath.Text = path;
            string[] dirs = Directory.GetDirectories(path);
            lstFiles.Items.Add("..");
            foreach (string dir in dirs)
            {
                DirectoryInfo di = new DirectoryInfo(dir);
                string name = System.IO.Path.GetDirectoryName(dir);
                lstFiles.Items.Add(di.Name);
            }
          
        }
        string getAncestorPath(int n, string path)
        {
            for (int i = 0; i < n; ++i)
                path = Directory.GetParent(path).FullName;
            return path;
        }

        private void Browse_Click(object sender, RoutedEventArgs e)
        {

            System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                WDirectory = dialog.SelectedPath;
                txtPath.Text = WDirectory;
            }
            LoadNavTab(WDirectory);
        }
        private void BtnPublish_Click(object sender, RoutedEventArgs e)
        {
            
            List <String> argv = new List<string>();
            argv.Add("CodePublisher.exe");
            argv.Add(txtPath.Text);

            if (cbRecurse.IsChecked.Value)
            {
                argv.Add("/s");

            }
            argv.Add("/demo");
            string[] patterns = txtPatterns.Text.Split(' ');
            for (int i = 0; i < patterns.Length; i++)
            {
                argv.Add(patterns[i]);
            }
            argv.Add(txtRegex.Text);

            Shim shim = new Shim();
            shim.SetCmd(argv);
            List<String> converteredList = shim.GetConvertedFiles();
            cvtdFiles.Items.Clear();
            for(int i=0;i< converteredList.Count; i++)
            {
                cvtdFiles.Items.Add(converteredList[i]);
            }

            // List<String> listConvertedFiles;    
        }


        /* private void btnPath_TextChanged(object sender, RoutedEventArgs e)
         {

             lstFiles.Items.Clear();
             lstFiles.Items.Add("..");
             var dirs = Directory.GetDirectories(WDirectory);
             foreach (var dir in dirs)
             {
                 string dirName
                 lstFiles.Items.Add(Path.GetFileName(dir));
             }
             List<string> filesMatchingPatterns = new List<string>();
             var patterns = Patterns.split(' ');
             foreach (var patt in patterns)
                 filesMatchingPatterns.AddRange(Directory.GetFiles(WDirectory, patt));
             else{
                 foreach (var file in filesMatchingPatterns) lstFiles.Items.Add(Path.GetFileName(file));
             }

         }*/

        private void LstFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {        
            string txt1 = System.IO.Path.Combine(txtPath.Text, lstFiles.SelectedItem.ToString());

            if (Directory.Exists(txt1))
            {

                lstFiles.Items.Clear();

                txtPath.Text = System.IO.Path.GetFullPath(txt1);
                string[] dirs = Directory.GetDirectories(txt1);
                lstFiles.Items.Add("..");
                foreach (string dir in dirs)
                {
                    DirectoryInfo di = new DirectoryInfo(dir);
                    string name = System.IO.Path.GetDirectoryName(dir);
                    lstFiles.Items.Add(di.Name);
                }

                string[] files = Directory.GetFiles(txt1);
                foreach (string file in files)
                {
                    string name = System.IO.Path.GetFileName(file);
                    lstFiles.Items.Add(name);
                }
            }
            else
            {
                //Do nothing if the clicked value is not a directory
            }

        }

      
        private void TxtPatterns_KeyUp(object sender, KeyEventArgs e)
        {

        }

        private void CvtdFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            String str = cvtdFiles.SelectedItem.ToString();
            System.Diagnostics.Process.Start(str);
        }
    }
}
