#include "../utils.h"
#include "../Parser.h"
#include "../hnswalg.h"

using namespace hnswlib;

static float test_approx(float *massQ, size_t nq,  HierarchicalNSW *quantizer, size_t d,
                         std::vector<std::priority_queue< std::pair<float,  idx_t >>> &answers, size_t k)
{
    //int res[k];
    //std::ofstream out("triplet_gt.ivecs", std::ios::binary);

    //for (int i = 0; i < nq; i++) {
    //    int j = k-1;
    //    while (result.size()) {
    //        res[j--] = result.top().second
    //        result.pop();
    //    }
    //    out.write((char *) &k, sizeof(uint32_t));
    //    out.write((char *) res, k * sizeof(uint32_t));
    //}

    size_t correct = 0;
    size_t total = 0;
    for (int i = 0; i < nq; i++) {
        std::priority_queue< std::pair<float,  idx_t >> result = quantizer->searchKnn(massQ + d*i, k);
        std::priority_queue<std::pair<float, idx_t >> gt(answers[i]);
        std::unordered_set<idx_t> g;
        total += gt.size();
        while (gt.size()) {
            g.insert(gt.top().second);
            gt.pop();
        }
        while (result.size()) {
            if (g.find(result.top().second) != g.end())
                correct++;
            result.pop();
        }
    }
    return 1.0f * correct / total;
}

static void test_vs_recall(float *massQ, size_t nq,  HierarchicalNSW *quantizer,
                           size_t d, std::vector<std::priority_queue< std::pair<float,  idx_t>>> &answers, size_t k)
{
    std::vector<size_t> efs;// = {k};
    if (k < 10) {
        for (int i = k; i < 10; i++) efs.push_back(i);
      	for (int i = 10; i < 100; i += 10) efs.push_back(i);
	for (int i = 100; i <= 500; i += 40) efs.push_back(i);
    } else if (k < 100) {
        for (int i = k; i < 100; i += 10) efs.push_back(i);
        for (int i = 100; i <= 500; i += 40) efs.push_back(i);
    } else {
        for (int i = k; i <= 500; i += 40) efs.push_back(i);
    }
    for (size_t ef : efs) {
        quantizer->efSearch = ef;
        quantizer->dist_calc = 0;
        quantizer->hops = 0;
        StopW stopw =  StopW();
        float recall = test_approx(massQ, nq, quantizer, d, answers, k);
        float time_us_per_query = stopw.getElapsedTimeMicro() / nq;
        float avg_dists = quantizer->dist_calc*1.f / nq;
	float avg_hops = quantizer->hops *1.f / nq;
        std::cout << ef << "\t" << recall << "\t" << time_us_per_query << " us\t" << avg_dists << " dcs\t" << avg_hops << " hps\n";
    }
}

int main(int argc, char **argv)
{
    //===============
    // Parse Options
    //===============
    Parser opt = Parser(argc, argv);

    //==================
    // Load Groundtruth
    //==================
    std::cout << "Loading groundtruth from " << opt.path_gt << std::endl;
    std::vector<idx_t> massQA(opt.nq * opt.ngt);
    {
        std::ifstream gt_input(opt.path_gt, std::ios::binary);
        readXvec<idx_t>(gt_input, massQA.data(), opt.ngt, opt.nq);
    }

    //==============
    // Load Queries
    //==============
    std::cout << "Loading queries from " << opt.path_q << std::endl;
    std::vector<float> massQ(opt.nq * opt.d);
    {
        std::ifstream query_input(opt.path_q, std::ios::binary);
        readXvec<float>(query_input, massQ.data(), opt.d, opt.nq);
    }

    //===========
    // Build NSG
    //===========
    HierarchicalNSW *quantizer;
    if (exists(opt.path_edges)) {
        quantizer = new HierarchicalNSW(opt.nb, opt.d, opt.path_base, opt.path_edges, false);
        quantizer->efSearch = opt.efConstruction;
    }
	quantizer->limit = opt.limit;

    //===================
    // Parse groundtruth
    //===================
    std::cout << "Parsing groundtruth" << std::endl;
    std::vector<std::priority_queue< std::pair<float,  idx_t >>> answers;
    (std::vector<std::priority_queue< std::pair<float, idx_t >>>(opt.nq)).swap(answers);
    for (size_t i = 0; i < opt.nq; i++)
        answers[i].emplace(0.0f, massQA[opt.ngt*i]);

    test_vs_recall(massQ.data(), opt.nq, quantizer, opt.d, answers, opt.k);

    delete quantizer;
}
