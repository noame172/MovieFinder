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
using System.Windows.Shapes;

namespace Client
{
    /// <summary>
    /// Interaction logic for ShowMovie.xaml
    /// </summary>
    public class MovieData
    {
        public string title;
        public string overview;
        public float runTimeInMinutes;
        public List<string> genres;

        public MovieData(string title, string overview, float runTimeInMinutes, List<string> genres)
        {
            this.title = title;
            this.overview = overview;
            this.runTimeInMinutes = runTimeInMinutes;
            this.genres = genres;
        }
    }
    public partial class ShowMovieWindow : Window
    {
        private Communicator m_communicator;
        private MovieData m_movieData;
        public ShowMovieWindow(Communicator communicator, MovieData movieData)
        {
            InitializeComponent();
            m_communicator = communicator;
            m_movieData = movieData;
            if (m_movieData != null) loadMovieDetails();
            else MessageBox.Show("No movie data received!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        private void loadMovieDetails()
        {
            txtMovieGenres.Text = string.Join(", ", m_movieData.genres);
            txtMovieOverview.Text = m_movieData.overview;
            txtMovieRuntime.Text = m_movieData.runTimeInMinutes.ToString() + " minutes";
            txtMovieTitle.Text = m_movieData.title;
        }
        private void btnBack_Click(object sender, RoutedEventArgs e)
        {
            MainWindow mainWindow = new MainWindow(m_communicator);
            mainWindow.Show();
            this.Close();

        }
    }
}
