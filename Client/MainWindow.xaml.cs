using System.Diagnostics;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Communicator m_communicator;
        public MainWindow()
        {
            InitializeComponent();
            m_communicator = new Communicator();
            this.Loaded += MainWindow_Loaded;
        }
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                m_communicator.connect("127.0.0.1", 5000);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to connect to server: {ex.Message}", "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
        public MainWindow(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
        }
        private void btnSearch_Click(object sender, RoutedEventArgs e)
        {
            string movieName = txtMovieName.Text;
            if (!string.IsNullOrWhiteSpace(movieName))
            {
                Response response = m_communicator.sendAndReceive(RequestCode.GetMovie, new { movieName = movieName });
                if (response != null)
                {
                    if (response.title == "" || response.runTimeInMinutes == 0 || response.genres == null || response.overview == "")
                    {
                        ErrorWindow errorWindow = new ErrorWindow(m_communicator);
                        errorWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        ShowMovieWindow showMovieWindow = new ShowMovieWindow(m_communicator, new MovieData(response.title,
                            response.overview,
                            response.runTimeInMinutes,
                            response.genres,
                            response.moviePosterPath));
                        showMovieWindow.Show();
                        this.Close();

                    }
                }
            }
            else
            {
                MessageBox.Show("couldnt find this movie name.", "Input Error", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }
    }
}