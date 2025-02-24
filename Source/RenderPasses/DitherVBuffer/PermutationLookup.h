#pragma once
#include "Falcor.h"

template<int T>
inline int torusDistance(int x1, int y1, int x2, int y2) {
    int dx = std::min<int>(std::abs(x1 - x2), T - std::abs(x1 - x2));
    int dy = std::min<int>(std::abs(y1 - y2), T - std::abs(y1 - y2));
    return dx + dy;
}

template<int T>
inline double scorePermutation(const std::array<int, T* T>& indices) {
    // 2D Distance Score
    double sumInverseDist = 0.0;
    for (size_t i = 0; i < T * T; ++i) {
        for (size_t j = i + 1; j < T * T; ++j) {
            int x1 = i % T, y1 = i / T;
            int x2 = j % T, y2 = j / T;
            int valueDiff = std::abs(indices[i] - indices[j]);
            int torusDist = torusDistance<T>(x1, y1, x2, y2);

            if (torusDist > 0) {
                sumInverseDist += (double)valueDiff / torusDist;
            }
        }
    }

    // 1D Offset Score (with Wrap-Around)
    int min1DSpacing = std::numeric_limits<int>::max();
    int sum1DSpacing = 0;
    for (size_t i = 0; i < T * T; ++i) {
        size_t nextIndex = (i + 1) % (T * T);  // Wrap-around at the end
        int spacing = std::abs(indices[i] - indices[nextIndex]);
        min1DSpacing = std::min(min1DSpacing, spacing);
        sum1DSpacing += spacing;
    }

    // Combine Scores (adjust weights as needed)
    return sumInverseDist + min1DSpacing * T * T + sum1DSpacing * 3.0;
}

template<int T>
inline std::vector<std::array<int, T* T>> generateBestPermutations(size_t maxResults = 10) {
    std::array<int, T * T> indices;
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<std::pair<double, std::array<int, T* T>>> scoredPermutations;

    do {
        double score = scorePermutation<T>(indices);
        scoredPermutations.push_back({ score, indices });
    } while (std::next_permutation(indices.begin(), indices.end()));

    // Sort permutations by best score (higher is better)
    std::sort(scoredPermutations.begin(), scoredPermutations.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    // Return only the best results
    std::vector<std::array<int, T* T>> bestPermutations;
    for (size_t i = 0; i < std::min(maxResults, scoredPermutations.size()); ++i) {
        bestPermutations.push_back(scoredPermutations[i].second);
    }
    std::cerr << "Max Inverse Dist" << invDistMax << std::endl;
    std::cerr << "Max Sum" << sumMax << std::endl;

    return bestPermutations;
}

template<int T>
inline void generatePermutations()
{
    auto bestPermutations = generateBestPermutations<T>(10);

    std::cout << "Top " << bestPermutations.size() << " Permutations:\n";
    for (const auto& perm : bestPermutations) {
        for (size_t i = 0; i < T; i++) {
            for (size_t j = 0; j < T; j++) {
                std::cout << perm[i * T + j] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "-----\n";
    }
}
