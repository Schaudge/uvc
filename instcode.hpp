
#undef INDELTYPE
#if INDEL_ID == 1
#define INDELTYPE std::string
#else
#define INDELTYPE uint32_t
#endif

// this code is instantiated multiple times, with INDELTYPE as instantiation parameter.
// template <AlignmentSymbol link1, AlignmentSymbol link2, AlignmentSymbol link3p> // , LinkType linkType>
std::array<unsigned int, 2>
#if INDEL_ID == 1
fill_by_indel_info2_1
#else
fill_by_indel_info2_2
#endif
(bcfrec::BcfFormat & fmt,
        const Symbol2CountCoverageSet & symbol2CountCoverageSet, 
        const unsigned int strand, 
        const unsigned int refpos, 
        const AlignmentSymbol symbol,
        const std::map<std::uint32_t, std::map<INDELTYPE, uint32_t>> & bq_tsum_depth,
        const std::map<std::uint32_t, std::map<INDELTYPE, uint32_t>> & fq_tsum_depth,
        const std::string & refchars, 
        const std::string & repeatunit, 
        unsigned int repeatnum) {
    
    assert(isSymbolIns(symbol) || isSymbolDel(symbol));
    if (isSymbolIns(symbol)) {
        assert (LINK_I1 == symbol || LINK_I2 == symbol || LINK_I3P == symbol || 
                !fprintf(stderr, "Symbol %s does not match any of {%s, %s, %s}", 
                SYMBOL_TO_DESC_ARR[symbol], SYMBOL_TO_DESC_ARR[LINK_I1], SYMBOL_TO_DESC_ARR[LINK_I2], SYMBOL_TO_DESC_ARR[LINK_I3P]));
    } else {
        assert (LINK_D1 == symbol || LINK_D2 == symbol || LINK_D3P == symbol || 
                !fprintf(stderr, "Symbol %s does not match any of {%s, %s, %s}", 
                SYMBOL_TO_DESC_ARR[symbol], SYMBOL_TO_DESC_ARR[LINK_D1], SYMBOL_TO_DESC_ARR[LINK_D2], SYMBOL_TO_DESC_ARR[LINK_D3P]));
    }
    assert(bq_tsum_depth.find(refpos) != bq_tsum_depth.end());
    
    std::vector<std::tuple<uint32_t, uint32_t, std::string>> bqfq_depth_mutform_tuples;
    for (auto indel2data4 : bq_tsum_depth.at(refpos)) {
        const auto indel = indel2data4.first;
#if INDEL_ID == 1
        const std::string indelstring = indel2data4.first;
#else
        const std::string indelstring = refchars.substr(refpos - symbol2CountCoverageSet.getUnifiedIncluBegPosition(), indel2data4.first); 
#endif
        if (indelstring.size() == 0) {
            continue;
        }
        
        const uint32_t bqdata = posToIndelToData_get(bq_tsum_depth, refpos, indel);
        const uint32_t fqdata = posToIndelToData_get(fq_tsum_depth, refpos, indel);
        assert(bqdata > 0);
        bqfq_depth_mutform_tuples.push_back(std::make_tuple(fqdata, bqdata, indelstring));
    }
    unsigned int gapbAD1sum = 0;
    unsigned int gapcAD1sum = 0;
    std::sort(bqfq_depth_mutform_tuples.rbegin(), bqfq_depth_mutform_tuples.rend());
    auto & gapN = ((0 == strand) ? fmt.gapNf : fmt.gapNr);
    gapN.push_back(bqfq_depth_mutform_tuples.size());
    unsigned int prev_gapseq_len = 0;
    unsigned int prev_gap_cAD = 0;
    unsigned int maxdiff = 0; 
    for (auto bqfq_depth_mutform : bqfq_depth_mutform_tuples) {
        const auto gap_seq = std::get<2>(bqfq_depth_mutform);
        assert(gap_seq.size() > 0);
        auto gap_cAD = std::get<0>(bqfq_depth_mutform);
        auto gap_bAD = std::get<1>(bqfq_depth_mutform);
        fmt.gapSeq.push_back(gap_seq);
        fmt.gapbAD1.push_back(gap_bAD);
        fmt.gapcAD1.push_back(gap_cAD);
        if (gap_seq.size() != prev_gapseq_len && prev_gap_cAD > gap_cAD) {
            maxdiff = MAX(maxdiff, prev_gap_cAD - gap_cAD);
        }
        prev_gapseq_len = gap_seq.size();
        prev_gap_cAD = gap_cAD;
        gapbAD1sum += gap_bAD;
        gapcAD1sum += gap_cAD;
    }
    return {MAX(maxdiff, prev_gap_cAD), gapcAD1sum};
    
    // this is a rare case of indel inconsistency:
    /*
    if (fmt.gapbAD1[strand] != gapbAD1sum) {
        std::string msg = std::to_string(strand) + "\t" + std::to_string(refpos) + "\t" + std::to_string(symbol);
        bcfrec::streamAppendBcfFormat(msg, fmt);
        std::cerr << msg << "\n";
    }
    */
    // "/4+16" is a probabilistic check in the following code
    /*
    assert(fmt.AD2[strand] >= gapAD2sum && (fmt.AD2[strand] <= gapAD2sum * 5 / 4 + 16) ||
            !(std::cerr << fmt.AD2[strand] << " >= | <=5/4+16 " << gapAD2sum
            << " failed for AD2 and gapAD2sum for strand " << strand << " at position " << refpos << " for symbol " << SYMBOL_TO_DESC_ARR[symbol] 
            << " and gapNum " << fmt.gapNum[strand] << " or equiv " << rawdu2_dedup_size1_mutform_tuples.size() << std::endl));
    assert(fmt.ADr[strand] >= gapADrsum && (fmt.ADr[strand] <= gapADrsum * 5 / 4 + 16) ||
            !(std::cerr << fmt.ADr[strand] << " >= | <=5/4+16 " << gapADrsum
            << " failed for ADr and gapADrsum for strand " << strand << " at position " << refpos << " for symbol " << SYMBOL_TO_DESC_ARR[symbol] << " and gapNum " << fmt.gapNum[strand] << std::endl));
    */
};

