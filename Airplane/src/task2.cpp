#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <cmath>
#include <array>
#define dividing_angle 17
#include "classifier.h"
#include "EasyBMP.h"
#include "linear.h"
#include "argvparser.h"
#include "matrix.h"

using std::string;
using std::vector;
using std::array;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;
using std::endl;

using CommandLineProcessing::ArgvParser;

typedef vector<pair<BMP*, int> > TDataSet;
typedef vector<pair<string, int> > TFileList;
typedef vector<pair<vector<float>, int> > TFeatures;

// Load list of files and its labels from 'data_file' and
// stores it in 'file_list'
void LoadFileList(const string& data_file, TFileList* file_list) {
    ifstream stream(data_file.c_str());

    string filename;
    int label;
    
    int char_idx = data_file.size() - 1;
    for (; char_idx >= 0; --char_idx)
        if (data_file[char_idx] == '/' || data_file[char_idx] == '\\')
            break;
    string data_path = data_file.substr(0,char_idx+1);
    
    while(!stream.eof() && !stream.fail()) {
        stream >> filename >> label;
        if (filename.size())
            file_list->push_back(make_pair(data_path + filename, label));
    }

    stream.close();
}

// Load images by list of files 'file_list' and store them in 'data_set'
void LoadImages(const TFileList& file_list, TDataSet* data_set) {
    for (size_t img_idx = 0; img_idx < file_list.size(); ++img_idx) {
            // Create image
        BMP* image = new BMP();
            // Read image from file
        image->ReadFromFile(file_list[img_idx].first.c_str());
            // Add image and it's label to dataset
        data_set->push_back(make_pair(image, file_list[img_idx].second));
    }
}

// Save result of prediction to file
void SavePredictions(const TFileList& file_list,
                     const TLabels& labels, 
                     const string& prediction_file) {
        // Check that list of files and list of labels has equal size 
    assert(file_list.size() == labels.size());
        // Open 'prediction_file' for writing
    ofstream stream(prediction_file.c_str());

        // Write file names and labels to stream
    for (size_t image_idx = 0; image_idx < file_list.size(); ++image_idx)
        stream << file_list[image_idx].first << " " << labels[image_idx] << endl;
    stream.close();
}

float norm_vector(array<float, dividing_angle> a)
{
    float sum = 0.0;
    for(int i = 0; i < dividing_angle; i++)
        sum += a[i] * a[i];
    //if(sum < 0)
    //    cout << "test" << endl;
    return sqrt(sum);
}

// Exatract features from dataset.
// You should implement this function by yourself =)
void ExtractFeatures(const TDataSet& data_set, TFeatures* features) {
    const int dividing_width = 8, dividing_height = 8; //, dividing_angle = 17;
    BMP* image;
    RGBApixel pixel;
    const double PI = 3.14159265358979;
    Matrix<array <float, dividing_angle>> histograms(dividing_height, dividing_width);
    array<float, dividing_angle> rest_hist;
    int s = 0, image_height = 0, image_width = 0, element_width = 0, element_height = 0, rest_width = 0, rest_height = 0;
    double norm = 0.0;
    Matrix<array<int, 3>> colors(dividing_height, dividing_width);
    
    for (size_t image_idx = 0; image_idx < data_set.size(); ++image_idx) {

        image = data_set[image_idx].first;
        image_width = image -> TellWidth(), image_height = image -> TellHeight();
        
        Matrix<int> brightness(image_height, image_width);
        Matrix<int> grad_x(image_height - 2, image_width - 2);
        Matrix<int> grad_y(image_height - 2, image_width - 2);
        Matrix<int> grad_r(image_height - 2, image_width - 2);
        Matrix<double> grad_angle(image_height - 2, image_width - 2);
        
        for(int i = 0; i < image_height; i++)
            for(int j = 0; j < image_width; j++)
            {
                pixel = image -> GetPixel(j, i);
                s = 0.299 * pixel.Red + 0.114 * pixel.Blue + 0.587 * pixel.Green;
                brightness(i, j) = s;
            }
        
        for(int i = 0; i < image_height - 2; i++)
            for(int j = 0; j < image_width - 2; j++)
            {
                grad_x(i, j) = brightness(i + 1, j + 2) - brightness(i + 1, j);
                grad_y(i, j) = brightness(i, j + 1) - brightness(i + 2, j + 1);
            }
        // calculating gradients
        for(int i = 0; i < image_height - 2; i++)
            for(int j = 0; j < image_width - 2; j++) {
                grad_r(i, j) = grad_x(i, j) * grad_x(i, j) + grad_y(i, j) * grad_y(i, j);
                grad_angle(i, j) = atan2(grad_y(i, j), grad_x(i, j));
                //cout << grad_r(i, j) << " " << grad_angle(i, j) << " ";
            }
        
        
        element_width = (image_width - 2) / dividing_width;
        element_height = (image_height - 2) / dividing_height;
        
        for(int i = 0; i < dividing_height; i++)
            for(int j = 0; j < dividing_width; j++)
                for(int k = 0; k < dividing_angle; k++)
                    histograms(i, j)[k] = 0.0;

        for(int i = 0; i < dividing_height; i++)
            for(int j = 0; j < dividing_width; j++) {
                for(int k = 0; k < element_width; k++)
                    for(int l = 0; l < element_height; l++) {
                        // building histogram
                        histograms(i, j)[floor(((grad_angle(i * element_height + l, j * element_width + k) + PI) / (2.0 * PI)) * (dividing_angle - 1))]++;
                        pixel = image -> GetPixel(j * element_width + k, i * element_height + l);
                        colors(i, j)[0] += pixel.Red;
                        colors(i, j)[1] += pixel.Green;
                        colors(i, j)[2] += pixel.Blue;
                    }
                // calculating average coloer
                colors(i, j)[0] /= element_width * element_height;
                colors(i, j)[1] /= element_width * element_height;
                colors(i, j)[2] /= element_width * element_height;
            }
        
        for(int i = 0; i < dividing_angle; i++)
            rest_hist[i] = 0.0;

        rest_width = (image_width - 2) % dividing_width;
        rest_height = (image_height - 2) % dividing_height;
        
        // building histogram for pixels from the right and bottom
        if(rest_width > 0)
            for(int i = 0; i < image_height - 2 - rest_height; i++)
                for(int j = image_width - 2 - rest_width; j < image_width - 2; j++) {
                    rest_hist[floor(((grad_angle(i, j) + PI) / (2.0 * PI)) * (dividing_angle - 1))]++;
                    //cout << i / (element_height) << " ";
                }
        
        if(rest_height > 0)
            for(int i = image_height - 2 - rest_height; i < image_height - 2; i++)
                for(int j = 0; j < image_width - 2; j++) {
                    rest_hist[floor(((grad_angle(i, j) + PI) / (2.0 * PI)) * (dividing_angle - 1))]++;
                    //cout << j / (element_width) << " ";
                }
        norm = norm_vector(rest_hist);
        // normalizing histograms
        for(int k = 0; k < dividing_angle; k++)
            rest_hist[k] /= norm;
        
        
        for(int i = 0; i < dividing_height; i++)
            for(int j = 0; j < dividing_width; j++)
            {
                norm = norm_vector(histograms(i, j));
                for(int k = 0; k < dividing_angle; k++) {
                    histograms(i, j)[k] /= norm;
                }
            }
        
        
        vector<float> one_image_features;
        for(int i = 0; i < dividing_height; i++)
            for(int j = 0; j < dividing_width; j++)
                for(int k = 0; k < dividing_angle; k++)
                    one_image_features.push_back(histograms(i, j)[k]);
        
        // implementation of pyramid of deskriptors
        image_width = (image_width - 2) / 2;
        image_height = (image_height - 2) / 2;
        element_width = image_width / dividing_width;
        element_height = image_height / dividing_height;
        
        for(int m = 0; m < 2; m++)
            for(int n = 0; n < 2; n++)
            {
                for(int i = 0; i < dividing_height; i++)
                    for(int j = 0; j < dividing_width; j++)
                        for(int k = 0; k < dividing_angle; k++)
                            histograms(i, j)[k] = 0.0;
        
                for(int i = 0; i < dividing_height; i++)
                    for(int j = 0; j < dividing_width; j++) {
                        for(int k = 0; k < element_width; k++)
                            for(int l = 0; l < element_height; l++) {
                                histograms(i, j)[floor(((grad_angle(m * image_height + i * element_height + l, n * image_width + j * element_width + k) + PI) / (2.0 * PI)) * (dividing_angle - 1))]++;
                            }
                    }
                
                for(int i = 0; i < dividing_height; i++)
                    for(int j = 0; j < dividing_width; j++)
                    {
                        norm = norm_vector(histograms(i, j));
                        for(int k = 0; k < dividing_angle; k++) {
                            histograms(i, j)[k] /= norm;
                        }
                    }
                
                for(int i = 0; i < dividing_height; i++)
                    for(int j = 0; j < dividing_width; j++)
                        for(int k = 0; k < dividing_angle; k++)
                            one_image_features.push_back(histograms(i, j)[k]);
            }
        
        for(int i = 0; i < dividing_angle; i++)
            one_image_features.push_back(rest_hist[i]);
        
        for(int i = 0; i < dividing_height; i++)
            for(int j = 0; j < dividing_width; j++)
                for(int k = 0; k < 3; k++)
                    one_image_features.push_back(colors(i, j)[k] / 255.0);
        
        features->push_back(make_pair(one_image_features, data_set[image_idx].second));
        
        // End of sample code
    }
}

// Clear dataset structure
void ClearDataset(TDataSet* data_set) {
        // Delete all images from dataset
    for (size_t image_idx = 0; image_idx < data_set->size(); ++image_idx)
        delete (*data_set)[image_idx].first;
        // Clear dataset
    data_set->clear();
}

// Train SVM classifier using data from 'data_file' and save trained model
// to 'model_file'
void TrainClassifier(const string& data_file, const string& model_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // Model which would be trained
    TModel model;
        // Parameters of classifier
    TClassifierParams params;
    
        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // PLACE YOUR CODE HERE
        // You can change parameters of classifier here
    params.C = 0.01;
    TClassifier classifier(params);
        // Train classifier
    classifier.Train(features, &model);
        // Save model to file
    model.Save(model_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

// Predict data from 'data_file' using model from 'model_file' and
// save predictions to 'prediction_file'
void PredictData(const string& data_file,
                 const string& model_file,
                 const string& prediction_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // List of image labels
    TLabels labels;

        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // Classifier 
    TClassifier classifier = TClassifier(TClassifierParams());
        // Trained model
    TModel model;
        // Load model from file
    model.Load(model_file);
        // Predict images by its features using 'model' and store predictions
        // to 'labels'
    classifier.Predict(features, model, &labels);

        // Save predictions
    SavePredictions(file_list, labels, prediction_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

int main(int argc, char** argv) {
    // Command line options parser
    ArgvParser cmd;
        // Description of program
    cmd.setIntroductoryDescription("Machine graphics course, task 2. CMC MSU, 2014.");
        // Add help option
    cmd.setHelpOption("h", "help", "Print this help message");
        // Add other options
    cmd.defineOption("data_set", "File with dataset",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("model", "Path to file to save or load model",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("predicted_labels", "Path to file to save prediction results",
        ArgvParser::OptionRequiresValue);
    cmd.defineOption("train", "Train classifier");
    cmd.defineOption("predict", "Predict dataset");
        
        // Add options aliases
    cmd.defineOptionAlternative("data_set", "d");
    cmd.defineOptionAlternative("model", "m");
    cmd.defineOptionAlternative("predicted_labels", "l");
    cmd.defineOptionAlternative("train", "t");
    cmd.defineOptionAlternative("predict", "p");

        // Parse options
    int result = cmd.parse(argc, argv);

        // Check for errors or help option
    if (result) {
        cout << cmd.parseErrorDescription(result) << endl;
        return result;
    }

        // Get values 
    string data_file = cmd.optionValue("data_set");
    string model_file = cmd.optionValue("model");
    bool train = cmd.foundOption("train");
    bool predict = cmd.foundOption("predict");

        // If we need to train classifier
    if (train)
        TrainClassifier(data_file, model_file);
        // If we need to predict data
    if (predict) {
            // You must declare file to save images
        if (!cmd.foundOption("predicted_labels")) {
            cerr << "Error! Option --predicted_labels not found!" << endl;
            return 1;
        }
            // File to save predictions
        string prediction_file = cmd.optionValue("predicted_labels");
            // Predict data
        PredictData(data_file, model_file, prediction_file);
    }
}