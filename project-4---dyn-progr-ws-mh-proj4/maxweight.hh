////////////////////////////////////////////////////////////////////////////////
// maxweight.hh
//
// Compute the set of foods that maximizes the weight in foods, within 
// a given maximum calorie amount with the dynamic programming or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once


#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>


// One food item available for purchase.
class FoodItem
{
	//
	public:
		
		//
		FoodItem
		(
			const std::string& description,
			double calories,
            double weight_ounces
		)
			:
			_description(description),
			_calories(calories),
            _weight_ounces(weight_ounces)
		{
			assert(!description.empty());
			assert(calories > 0);
		}
		
		//
		const std::string& description() const { return _description; }
        double calorie() const { return _calories; }
		double weight() const { return _weight_ounces; }
		
	//
	private:
		
		// Human-readable description of the food, e.g. "spicy chicken breast". Must be non-empty.
		std::string _description;
		
		// Calories; Must be positive
		double _calories;
    
    // Food weight, in ounces; most be non-negative.
		double _weight_ounces;	
};


// Alias for a vector of shared pointers to FoodItem objects.
typedef std::vector<std::shared_ptr<FoodItem>> FoodVector;


// Load all the valid food items from the CSV database
// Food items that are missing fields, or have invalid values, are skipped.
// Returns nullptr on I/O error.
std::unique_ptr<FoodVector> load_food_database(const std::string& path)
{
	std::unique_ptr<FoodVector> failure(nullptr);
	
	std::ifstream f(path);
	if (!f)
	{
		std::cout << "Failed to load food database; Cannot open file: " << path << std::endl;
		return failure;
	}
	
	std::unique_ptr<FoodVector> result(new FoodVector);
	
	size_t line_number = 0;
	for (std::string line; std::getline(f, line); )
	{
		line_number++;
		
		// First line is a header row
		if ( line_number == 1 )
		{
			continue;
		}
		
		std::vector<std::string> fields;
		std::stringstream ss(line);
		
		for (std::string field; std::getline(ss, field, '^'); )
		{
			fields.push_back(field);
		}
		
		if (fields.size() != 3)
		{
			std::cout
				<< "Failed to load food database: Invalid field count at line " << line_number << "; Want 3 but got " << fields.size() << std::endl
				<< "Line: " << line << std::endl
				;
			return failure;
		}
		
		std::string
			descr_field = fields[0],
			calories_field = fields[1],
            weight_ounces_field = fields[2]
			;
		
		auto parse_dbl = [](const std::string& field, double& output)
		{
			std::stringstream ss(field);
			if ( ! ss )
			{
				return false;
			}
			
			ss >> output;
			
			return true;
		};
		
		std::string description(descr_field);
		double calories, weight_ounces;
		if (
			parse_dbl(calories_field, calories)
			&& parse_dbl(weight_ounces_field, weight_ounces)
		)
		{
			result->push_back(
				std::shared_ptr<FoodItem>(
					new FoodItem(
						description,
						calories,
                        weight_ounces
					)
				)
			);
		}
	}

	f.close();
	
	return result;
}


// Convenience function to compute the total weight and calories in 
// a FoodVector.
// Provide the FoodVector as the first argument
// The next two arguments will return the weight and calories back to 
// the caller.
void sum_food_vector
(
	const FoodVector& foods,
	double& total_calories,
    double& total_weight
)
{
	total_calories = total_weight = 0;
	for (auto& food : foods)
	{
		total_calories += food->calorie();
        total_weight += food->weight();
	}
}


// Convenience function to print out each FoodItem in a FoodVector,
// followed by the total weight and calories of it.
void print_food_vector(const FoodVector& foods)
{
	std::cout << "*** food Vector ***" << std::endl;
	
	if ( foods.size() == 0 )
	{
		std::cout << "[empty food list]" << std::endl;
	}
	else
	{
		for (auto& food : foods)
		{
			std::cout
				<< "Ye olde " << food->description()
				<< " ==> "
				<< "; calories = " << food->calorie()
                << "Weight of " << food->weight() << " ounces"
				<< std::endl
				;
		}
		
		double total_calories, total_weight;
		sum_food_vector(foods, total_calories, total_weight);
		std::cout
			<< "> Grand total calories: " << total_calories
			<< std::endl
            << "> Grand total weight: " << total_weight << " ounces" << std::endl
			;
	}
}


// Filter the vector source, i.e. create and return a new FoodVector
// containing the subset of the food items in source that match given
// criteria.
// This is intended to:
//	1) filter out food with zero or negative weight that are irrelevant to // our optimization
//	2) limit the size of inputs to the exhaustive search algorithm since it // will probably be slow.
//
// Each food item that is included must have at minimum min_weight and 
// at most max_weight.
//	(i.e., each included food item's weight must be between min_weight
// and max_weight (inclusive).
//
// In addition, the vector includes only the first total_size food items
// that match these criteria.
std::unique_ptr<FoodVector> filter_food_vector
(
	const FoodVector& source,
	double min_weight,
	double max_weight,
	int total_size
)
{
    std::unique_ptr<FoodVector> result(new FoodVector);		//create a new FoodVector

	//for loop to go through the food items in the vector source
	for (const auto& food : source) 
	{
		if (food->weight() >= min_weight && food->weight() <= max_weight) 	//each food item must have at minimum min_weight and max_weight
		{
			result->push_back(food);	//if conditions are satisfied, add the food item to the new FoodVector

			if (result->size() == total_size) 	//In addition, the vector only includes the first total_size food items that match the criteria
			{
				break;
			}
		}
	}
	
	return result;	//return new FoodVector that have been filtered out
}


// Compute the optimal set of food items with dynamic programming.
// Specifically, among the food items that fit within a total_calories,
// choose the foods whose weight-per-calorie is largest.
// Repeat until no more food items can be chosen, either because we've 
// run out of food items, or run out of space.
std::unique_ptr<FoodVector> dynamic_max_weight
(
	const FoodVector& foods,
	double total_calories
)
{
	// Initialize the variables
    int n = foods.size();
    int total_calories_int = (int)total_calories;	//convert total_calories to integer
    std::vector<std::vector<double>> T(n+1, std::vector<double>(total_calories_int + 1, 0));	//create a 2 dimensional vector 

    // Construct the dynamic programming 2 dimensional cache table
    for (int i = 1; i <= n; ++i)
    {
        int food_calorie = (int)foods[i-1]->calorie();
        double food_weight = foods[i-1]->weight();
        for (int j = 0; j <= total_calories_int; ++j)
        {
            if (food_calorie <= j)
            {
                T[i][j] = std::max(T[i-1][j], food_weight + T[i-1][j-food_calorie]);
            }
            else
            {
                T[i][j] = T[i-1][j];
            }
        }
    }

    // Construct the optimal food set
    std::unique_ptr<FoodVector> best(new FoodVector);	//will contain set of food items whose weight per calorie is largest
    int remaining_calories = total_calories_int;		//to keep track of how many calories allowed left
    for (int i = n; i > 0 && remaining_calories > 0; --i)	//for loop to go through the food items in reverse order
    {
        if (T[i][remaining_calories] != T[i-1][remaining_calories])
        {
            best->push_back(foods[i-1]);
            remaining_calories -= (int)foods[i-1]->calorie();	//update the remaining calories allowed
        }
    }
    
    return best;
}


// Compute the optimal set of food items with a exhaustive search algorithm.
// Specifically, among all subsets of food items, return the subset 
// whose weight in ounces fits within the total_weight one can carry and
// whose total calories is greatest.
// To avoid overflow, the size of the food items vector must be less than 64.
std::unique_ptr<FoodVector> exhaustive_max_weight
(
	const FoodVector& foods,
	double total_calorie
)
{
	//set n variable to store the size of foods
	int n = foods.size();
	//set best to none
	std::unique_ptr<FoodVector> best(nullptr);
	double best_weight = 0;

	//subset ranges from 0 to 2^n - 1
	for (int subset = 0; subset < (1 << n); ++subset) 
	{
		//FoodVector to store food items in current subset
		std::unique_ptr<FoodVector> current_subset(new FoodVector);
		double subset_weight = 0;
		double subset_calorie = 0;
		
		//loop to iterate through the current subset and access the food items
		for (int j = 0; j < n; ++j) 
		{
			if (subset & (1 << j)) //check if included in the current subset
			{
				subset_weight += foods[j]->weight();
				subset_calorie += foods[j]->calorie();
				current_subset->push_back(foods[j]);
			}
		}

		//if current subset calorie does not exceed the total calorie and subset weight is greater than the best weight stored so far,
		//set the best to the current subset
		if (subset_calorie <= total_calorie && (best == nullptr || subset_weight > best_weight)) 
		{
			best_weight = subset_weight;
			best = std::move(current_subset);
		}
	}
	
	return best;
}