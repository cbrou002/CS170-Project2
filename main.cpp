#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

vector<vector<double> > parseFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return vector<vector<double> >();
    }

    vector<vector<double> > data; 
    string line;

    while (getline(file, line)) { 
        istringstream iss(line);
        vector<double> row;
        double value;

        while (iss >> value) { 
            row.push_back(value);
        }

        data.push_back(row); 
    }

    file.close();
    return data;
}

double leave_one_out_cross_validation(vector<vector<double> > data, vector<double> current_set, int feature_to_add){
    int number_correctly_classified = 0;
    for(int i = 0; i < data.size(); i++){
        for(int j = 0; j < data[0].size(); j++){
            if(find(current_set.begin(), current_set.end(), j) == current_set.end() && j != feature_to_add){
                if(j != 0){
                    data[i][j] = 0;
                }
            }
        }   
    }
    for(int i = 0; i < data.size(); i++){
        vector<double> object_to_classify(data[i].begin() + 1, data[i].end());
        double label_object_to_classify = data[i][0];
        double nearest_neighbor_distance = numeric_limits<double>::infinity();
        double nearest_neighbor_location = numeric_limits<double>::infinity();
        int nearest_neighbor_label = -1;
        
        for(int k = 0; k < data.size(); k++){
            if(k != i){
                //cout << "Ask if " << i+1 << " is nearest neighbour with " << k+1 << endl;
                vector<double> other_object(data[k].begin() + 1, data[k].end()); // Features of the k-th row

                // Calculate the Euclidean distance
                double distance = 0.0;
                vector<double> temp_features = current_set;
                temp_features.push_back(feature_to_add);
                for (int f : temp_features) {
                    distance += pow(object_to_classify[f-1] - other_object[f-1], 2);
                }
                distance = sqrt(distance);
                if(distance < nearest_neighbor_distance){
                    nearest_neighbor_distance = distance;
                    nearest_neighbor_location = k;
                    nearest_neighbor_label = data[nearest_neighbor_location][0];
                }
            }
        }
        if(label_object_to_classify == nearest_neighbor_label){
            number_correctly_classified +=1; 
        }
    }
    if(feature_to_add != -1){
        cout << "\tUsing feature(s) {";
        for(int i = 0; i < current_set.size(); i++){
            if(i != current_set.size()){
                cout << static_cast<int>(current_set[i]) << ",";
            }
            else{
                cout << static_cast<int>(current_set[i]);
            }
            
        }
        cout << feature_to_add << "}";
        cout << " accuracy is " << fixed << setprecision(1) << (static_cast<double>(number_correctly_classified) / data.size())*100.0 << "%\n";
    }
    return static_cast<double>(number_correctly_classified) / data.size();
}

double leave_one_out_backward(vector<vector<double> > data, vector<double> current_set, int feature_to_remove) {
    int number_correctly_classified = 0;

    // Create a copy of current features and remove the target feature
    vector<double> temp_features = current_set;
    temp_features.erase(remove(temp_features.begin(), temp_features.end(), feature_to_remove), temp_features.end());

    for (int i = 0; i < data.size(); ++i) {
        vector<double> object_to_classify(data[i].begin() + 1, data[i].end());
        double label = data[i][0];
        double nearest_neighbor_distance = numeric_limits<double>::infinity();
        int nearest_label = -1;

        for (int k = 0; k < data.size(); ++k) {
            if (k != i){; // Skip self
                vector<double> neighbor(data[k].begin() + 1, data[k].end());
                double distance = 0.0;
                // Compute distance using ONLY the retained features
                for (int f : temp_features) {
                    distance += pow(object_to_classify[f-1] - neighbor[f-1], 2);
                }
                distance = sqrt(distance);
                if (distance < nearest_neighbor_distance) {
                    nearest_neighbor_distance = distance;
                    nearest_label = data[k][0];
                }
            }
        }

        if (label == nearest_label) {
            number_correctly_classified++;
        }
    }

    double accuracy = static_cast<double>(number_correctly_classified) / data.size();
    if(feature_to_remove != -1 && feature_to_remove != 0){
        cout << "\tUsing feature(s) {";
        for (size_t i = 0; i < temp_features.size(); ++i) {
            cout << static_cast<int>(temp_features[i]);
            if (i != temp_features.size() - 1) cout << ",";
        }
        cout << "} accuracy is " << fixed << setprecision(1) << accuracy * 100.0 << "%\n";
    }
    
    return accuracy;
}

int main(){
    cout << "Welcome to Corey Broussard's Feature Selection Algorithm." << endl << endl;
    cout << "Type in the name of the file to test : ";
    srand(time(0));
    string file;
    cin >> file;
    cout << endl << "Type the number of the algorithm you want to run." << endl << "\t1) Forward Selection\n" << "\t2) Backward Elimination\n";
    int algorithmType;
    cin >> algorithmType;
    cout << endl;
    if(algorithmType == 1){//forward selection
        
        vector<double> best_subset;//best subset seen with the highest accuracy
        double overall_best_accuracy = 0.0; //accuracy of the best subset out of all of them
        vector<vector<double> > data = parseFile(file);
        vector<double> current_set_of_features(0);
        vector<double> all_features(0);
        //cout << "Running nearest neighbor with no features, using \"leaving-one-out\" evaluation, I get an accuracy of " << leave_one_out_cross_validation(data, all_features, -1)*100.0 << "%" << endl << endl;
        for(int i = 1; i <= data[0].size()-1; i++){
            all_features.push_back(i);
        }
        
        double best_so_far_accuracy = 0;//accuracy of the best subset out of the ones seen so far
        double feature_to_add_at_this_level = 0;
        cout << "This dataset has " << data[0].size()-1 << " features (not including the class attribute), with " << data.size() << " instances." << endl;
        cout << "Running nearest neighbor with all " << data[0].size()-1 << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << leave_one_out_cross_validation(data, all_features, -1)*100.0 << "%" << endl << endl;
        cout << "Beginning search." << endl;
        for(int i = 1; i <= data[0].size()-1; i++){
            best_so_far_accuracy = 0;
            feature_to_add_at_this_level = 0;
            for(int k = 1; k <= data[0].size()-1; k++){   
                if(find(current_set_of_features.begin(), current_set_of_features.end(), k) == current_set_of_features.end()){//if feature k is not already added
                    double accuracy = leave_one_out_cross_validation(data, current_set_of_features, k);
                    if(accuracy > best_so_far_accuracy){
                        best_so_far_accuracy = accuracy;
                        feature_to_add_at_this_level = k;
                    }
                }
            }
            if(feature_to_add_at_this_level != 0){
                if(current_set_of_features.size() < data[0].size() - 2){//dont print "Feature set" if there is only one feature
                    cout << "Feature set {";
                    for(int i = 0; i < current_set_of_features.size(); i++){
                        cout << static_cast<int>(current_set_of_features[i]);
                        cout << ",";
                    }
                    cout << static_cast<int>(feature_to_add_at_this_level) << "} was best, accuracy is " << best_so_far_accuracy*100 << "%" << endl;
                }
                
                current_set_of_features.push_back(feature_to_add_at_this_level);
                if (best_so_far_accuracy > overall_best_accuracy) {
                    overall_best_accuracy = best_so_far_accuracy;
                    best_subset = current_set_of_features; // Update best_subset
                }
            }
        }
        cout << "Finished search!! The best feature subset is {";
        for(int i = 0; i < best_subset.size(); i++){
            cout << static_cast<int>(best_subset[i]);
            if (i != best_subset.size() - 1) {
                cout << ",";
            }
        }
        cout << "}, which has an accuracy of " << fixed << setprecision(1) << overall_best_accuracy*100 << "%" << endl;
    }

    else if(algorithmType == 2){                  //backward selection
        vector<vector<double> > data = parseFile(file);
        vector<double> current_set_of_features;
        vector<double> best_subset;
        double overall_best_accuracy = 0.0;
        vector<double> all_features(0);
        //cout << "Running nearest neighbor with no features, using \"leaving-one-out\" evaluation, I get an accuracy of " << leave_one_out_backward(data, all_features, -1)*100.0 << "%" << endl << endl;
        for(int i = 1; i <= data[0].size()-1; i++){
            all_features.push_back(i);
        }
        cout << "This dataset has " << data[0].size()-1 << " features (not including the class attribute), with " << data.size() << " instances." << endl;
        cout << "Running nearest neighbor with all " << data[0].size()-1 << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << leave_one_out_backward(data, all_features, -1)*100.0 << "%" << endl << endl;
        // Initialize with all features (1-based)
        for (int i = 1; i <= data[0].size() - 1; i++) {
            current_set_of_features.push_back(i);
        }
        // Compute initial accuracy with all features
        overall_best_accuracy = leave_one_out_backward(data, current_set_of_features, 0); // 0 = no feature removed
        best_subset = current_set_of_features;
        //cout << "Initial accuracy with all features: " << overall_best_accuracy * 100 << "%\n";
        cout << "Beginning search.\n";

        for (int level = 1; level <= data[0].size() - 1; level++) {
            if (current_set_of_features.size() <= 1) {
                //cout << "Stopping search as only one feature remains.\n";
                break;
            }
            double best_so_far_accuracy = 0;
            double feature_to_remove = -1;

            //cout << "On level " << level << " of the search tree\n";

            // Evaluate removing each feature in current set
            for (int k : current_set_of_features) {
                //cout << "--Considering removing feature " << k << endl;
                double accuracy = leave_one_out_backward(data, current_set_of_features, k);
                if (accuracy >= best_so_far_accuracy) { // Use >= to prefer smaller subsets
                    best_so_far_accuracy = accuracy;
                    feature_to_remove = k;
                }
            }

            if (feature_to_remove != -1) {
                // Remove the feature
                current_set_of_features.erase(
                    remove(current_set_of_features.begin(), current_set_of_features.end(), feature_to_remove),
                    current_set_of_features.end()
                );

                // Update overall best subset if accuracy improves or stays the same with fewer features
                if (best_so_far_accuracy > overall_best_accuracy) {
                    overall_best_accuracy = best_so_far_accuracy;
                    best_subset = current_set_of_features;
                }
                cout << "Feature set {";
                for(int i = 0; i < current_set_of_features.size(); i++){
                    cout << static_cast<int>(current_set_of_features[i]);
                    if(i != current_set_of_features.size()-1){
                        cout << ",";
                    }
                }
                cout << "} was best, accuracy is " << best_so_far_accuracy*100 << "%" << endl;
            }
        }
        cout << "Finished search! Best feature subset: {";
        for (int i = 0; i < best_subset.size(); ++i) {
            cout << static_cast<int>(best_subset[i]);
            if (i != best_subset.size() - 1) cout << ",";
        }
        cout << "}, which has an accuracy of " << fixed << setprecision(1) << overall_best_accuracy * 100.0 << "%\n";
    }
    return 0;
}
