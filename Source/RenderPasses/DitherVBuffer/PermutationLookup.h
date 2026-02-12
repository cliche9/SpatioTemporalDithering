#pragma once
#include "Falcor.h"
#include <fstream>
#include <random>

template<int T>
inline int torusDistance(int x1, int y1, int x2, int y2) {
    int dx = std::min<int>(std::abs(x1 - x2), T - std::abs(x1 - x2));
    int dy = std::min<int>(std::abs(y1 - y2), T - std::abs(y1 - y2));
    return dx + dy;
}

template<int T>
inline double torusDistanceL2(int x1, int y1, int x2, int y2) {
    int dx = std::min<int>(std::abs(x1 - x2), T - std::abs(x1 - x2));
    int dy = std::min<int>(std::abs(y1 - y2), T - std::abs(y1 - y2));
    return std::sqrt(dx * dx + dy * dy);
}

template<int T>
inline int scorePermutation(const std::array<int, T* T>& indices) {
    // 2D Distance Score
    int sumInverseDist = 0.0;
    /*for (size_t i = 0; i < T * T; ++i) {
        for (size_t j = i + 1; j < T * T; ++j) {
            int x1 = i % T, y1 = i / T;
            int x2 = j % T, y2 = j / T;
            int valueDiff = std::abs(indices[i] - indices[j]);
            valueDiff *= valueDiff;
            int torusDist = torusDistance<T>(x1, y1, x2, y2);
            //double torusDist = torusDistanceL2<T>(x1, y1, x2, y2);

            if (torusDist == 1) {
                if (valueDiff == 1) return 0.0;
                sumInverseDist += (double)valueDiff / torusDist;
            }
        }
    }*/

    for (size_t i = 0; i < T; ++i) {
        for (size_t j = 0; j < T; ++j) {
            auto index = i + T * j;
            auto indexRight = (i + 1) % T + T * j;
            auto indexBot = i + T * ((j + 1) % T);
            auto diff1 = indices[index] - indices[indexRight];
            diff1 *= diff1;
            auto diff2 = indices[index] - indices[indexBot];
            diff2 *= diff2;

            if(diff1 == 1 || diff2 == 1) return 0.0;
            sumInverseDist += diff1 + diff2;
        }
    }

    return sumInverseDist;
}

template<int T>
inline std::vector<std::array<int, T* T>> generateBestPermutations(size_t maxResults = size_t(-1)) {
    std::array<int, T * T> indices;
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<std::pair<int, std::array<int, T* T>>> scoredPermutations;

    do {
        int score = scorePermutation<T>(indices);
        scoredPermutations.push_back({ score, indices });
    } while (std::next_permutation(indices.begin(), indices.end()));

    // Sort permutations by best score (higher is better)
    std::sort(scoredPermutations.begin(), scoredPermutations.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    /*
    double lastScore = 0;
    int count = 0;
    auto filename = "permutations.txt";
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& perm : scoredPermutations) {
            if (abs(perm.first - lastScore) > 0.01)
            {
                if (count > 0) file << "Total: " << count << "\n\n";

                lastScore = perm.first;
                count = 0;
                file << "Score: " << lastScore << "\n";
            }
            ++count;

            for (size_t i = 0; i < T; i++) {
                for (size_t j = 0; j < T; j++) {
                    file << perm.second[i * T + j] << " ";
                }
                file << "\n";
            }
            file << "-----\n";
        }
    }

    lastScore = 0;
    int printCount = 0;
    count = 0;
    // print a preview of the matrices
    for (const auto& perm : scoredPermutations) {
        if(abs(perm.first - lastScore) > 0.01)
        {
            if(count > 0) std::cout << "Total: " << count << "\n\n";

            printCount = 3;
            lastScore = perm.first;
            count = 0;
            std::cout << "Score: " << lastScore << "\n";
        }
        ++count;

        if(printCount <= 0) continue;
        --printCount;
        for (size_t i = 0; i < T; i++) {
            for (size_t j = 0; j < T; j++) {
                std::cout << perm.second[i * T + j] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "-----\n";
    }
    std::cout << "Total: " << count << "\n\n";
    */
    // Return only the best results
    std::vector<std::array<int, T * T>> bestPermutations;
    for (size_t i = 0; i < std::min(maxResults, scoredPermutations.size()); ++i) {
        if(scoredPermutations[i].first == 0.0) break; // stop when score falls to zero
        bestPermutations.push_back(scoredPermutations[i].second);
    }

    return bestPermutations;
}

// Function to pack 4 values (0-3) into a single uint32_t using 4 bits per value
inline uint32_t packPermutation2x2(const std::array<int, 4>& indices) {
    uint32_t packed = 0;
    for (size_t i = 0; i < 4; ++i) {
        packed |= (indices[i] & 0xF) << (i * 4);
    }
    return packed;
}

// Function to pack 8 values (0-8) into a single uint32_t using 4 bits per value
inline uint32_t packPermutation(const std::array<int, 9>& indices) {
    uint32_t packed = 0;
    for (size_t i = 0; i < 8; ++i) {
        packed |= (indices[i] & 0xF) << (i * 4);
    }
    return packed;
}

template<int T>
std::vector<std::array<int, T* T>> removeOverlapping(std::vector<std::array<int, T* T>> perms, size_t maxOverlap = 1)
{
    std::vector<std::array<int, T* T>> res;

    // Iterate through each candidate permutation.
    for (const auto& candidate : perms) {
        bool valid = true;

        // Compare candidate with every already accepted permutation.
        for (const auto& existing : res) {
            size_t overlapCount = 0;
            // Count the number of positions where both arrays are identical.
            for (size_t i = 0; i < candidate.size(); ++i) {
                if (candidate[i] == existing[i]) {
                    ++overlapCount;
                }
            }
            if (overlapCount > maxOverlap) {
                valid = false;
                break;  // Candidate overlaps too much with an existing permutation.
            }
        }

        // Add candidate if it meets the criteria with all accepted arrays.
        if (valid) {
            res.push_back(candidate);
        }
    }

    return res;
}


inline ref<Buffer> generatePermutations3x3(ref<Device> pDevice)
{
    // generate permutations, ranked from best to worst, exlcuding permutations with succesive values next to each other
    auto perms = generateBestPermutations<3>(288); // 
    std::cout << "Permutations without successive elements: " << perms.size() << "\n";
    //perms = removeOverlapping<3>(perms, 3);
    //std::cout << "Permutations after removing overlap: " << perms.size() << "\n";

    std::vector<uint32_t> packed(perms.size());
    std::transform(perms.begin(), perms.end(), packed.begin(), packPermutation);

    return Buffer::createStructured(pDevice, sizeof(packed[0]), packed.size(), ResourceBindFlags::ShaderResource, Buffer::CpuAccess::None, packed.data(), false);
}

inline ref<Buffer> generatePermutations3x3(ref<Device> pDevice, int minScore, int maxScore)
{
    std::array<int, 3 * 3> indices;
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<std::array<int, 3 * 3>> perms;

    do {
        int score = scorePermutation<3>(indices);
        if (score >= minScore && score <= maxScore)
            perms.push_back(indices);
    } while (std::next_permutation(indices.begin(), indices.end()));

    std::cout << "Permutations with score between " << minScore << " and " << maxScore << ": " << perms.size() << "\n";

    std::vector<uint32_t> packed(perms.size());
    std::transform(perms.begin(), perms.end(), packed.begin(), packPermutation);

    return Buffer::createStructured(pDevice, sizeof(packed[0]), packed.size(), ResourceBindFlags::ShaderResource, Buffer::CpuAccess::None, packed.data(), false);
}

template<int T = 3>
inline std::vector<int> getPermutationScores()
{
    std::array<int, T * T> indices;
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<int> scores;

    do {
        int score = scorePermutation<T>(indices);
        scores.push_back(score);
    } while (std::next_permutation(indices.begin(), indices.end()));

    // reduce to unique, sorted scores
    std::sort(scores.begin(), scores.end(), std::greater<>());
    scores.erase(std::unique(scores.begin(), scores.end()), scores.end());
    return scores;
}

// ============================================================================
// 2x2 Dither Matrix Generation
// ============================================================================

inline ref<Buffer> generatePermutations2x2(ref<Device> pDevice)
{
    // 2x2 has 4! = 24 permutations
    // Note: For 2x2, the "no successive elements" constraint is impossible to satisfy
    // because in a 2x2 torus, every cell is adjacent to every other cell.
    // So we generate ALL permutations without filtering.
    std::array<int, 4> indices;
    std::iota(indices.begin(), indices.end(), 0);
    
    std::vector<std::array<int, 4>> allPerms;
    do {
        allPerms.push_back(indices);
    } while (std::next_permutation(indices.begin(), indices.end()));
    
    std::cout << "2x2 Permutations (all): " << allPerms.size() << "\n";

    std::vector<uint32_t> packed(allPerms.size());
    std::transform(allPerms.begin(), allPerms.end(), packed.begin(), packPermutation2x2);

    return Buffer::createStructured(pDevice, sizeof(packed[0]), packed.size(), ResourceBindFlags::ShaderResource, Buffer::CpuAccess::None, packed.data(), false);
}

// ============================================================================
// 4x4 Dither Matrix Generation
// ============================================================================

// Structure to hold a 4x4 matrix packed into two uint32_t values
// Each value (0-15) uses 4 bits, so we need 16*4 = 64 bits = 2 uint32_t
struct Packed4x4Matrix {
    uint32_t low;   // First 8 values
    uint32_t high;  // Last 8 values
};

// Pack 16 values (0-15) into two uint32_t values
inline Packed4x4Matrix packPermutation4x4(const std::array<int, 16>& indices) {
    Packed4x4Matrix packed;
    packed.low = 0;
    packed.high = 0;
    for (size_t i = 0; i < 8; ++i) {
        packed.low |= (indices[i] & 0xF) << (i * 4);
        packed.high |= (indices[i + 8] & 0xF) << (i * 4);
    }
    return packed;
}

// Generate 4x4 permutations using random sampling (16! is too large for exhaustive search)
// Uses simulated annealing to find good permutations
inline std::vector<std::array<int, 16>> generateBestPermutations4x4(size_t maxResults, size_t iterations = 1000000) {
    std::vector<std::pair<int, std::array<int, 16>>> bestPerms;
    
    // Start with Bayer matrix as initial guess
    std::array<int, 16> current = {
        0, 8, 2, 10,
        12, 4, 14, 6,
        3, 11, 1, 9,
        15, 7, 13, 5
    };
    
    int currentScore = scorePermutation<4>(current);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_real_distribution<> disReal(0.0, 1.0);
    
    double temperature = 100.0;
    double coolingRate = 0.9999;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Create neighbor by swapping two random positions
        auto neighbor = current;
        int i = dis(gen);
        int j = dis(gen);
        if (i != j) {
            std::swap(neighbor[i], neighbor[j]);
        }
        
        int neighborScore = scorePermutation<4>(neighbor);
        
        // Accept if better or with probability based on temperature
        if (neighborScore > currentScore) {
            current = neighbor;
            currentScore = neighborScore;
        } else if (neighborScore > 0 && disReal(gen) < std::exp((neighborScore - currentScore) / temperature)) {
            current = neighbor;
            currentScore = neighborScore;
        }
        
        temperature *= coolingRate;
        
        // Store good permutations
        if (currentScore > 0) {
            bool found = false;
            for (const auto& p : bestPerms) {
                if (p.second == current) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                bestPerms.push_back({currentScore, current});
            }
        }
    }
    
    // Sort by score (higher is better)
    std::sort(bestPerms.begin(), bestPerms.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<std::array<int, 16>> result;
    for (size_t i = 0; i < std::min(maxResults, bestPerms.size()); ++i) {
        if (bestPerms[i].first == 0) break;
        result.push_back(bestPerms[i].second);
    }
    
    return result;
}

inline ref<Buffer> generatePermutations4x4(ref<Device> pDevice, size_t maxResults = 256, size_t iterations = 1000000)
{
    auto perms = generateBestPermutations4x4(maxResults, iterations);
    std::cout << "4x4 Permutations found: " << perms.size() << "\n";
    
    if (perms.empty()) {
        // Fallback: use Bayer matrix if no good permutations found
        perms.push_back({
            0, 8, 2, 10,
            12, 4, 14, 6,
            3, 11, 1, 9,
            15, 7, 13, 5
        });
    }

    std::vector<Packed4x4Matrix> packed(perms.size());
    std::transform(perms.begin(), perms.end(), packed.begin(), packPermutation4x4);

    return Buffer::createStructured(pDevice, sizeof(packed[0]), packed.size(), ResourceBindFlags::ShaderResource, Buffer::CpuAccess::None, packed.data(), false);
}

// Get permutation scores for 2x2 and 4x4
template<>
inline std::vector<int> getPermutationScores<2>()
{
    std::array<int, 4> indices;
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<int> scores;

    do {
        int score = scorePermutation<2>(indices);
        scores.push_back(score);
    } while (std::next_permutation(indices.begin(), indices.end()));

    std::sort(scores.begin(), scores.end(), std::greater<>());
    scores.erase(std::unique(scores.begin(), scores.end()), scores.end());
    return scores;
}
