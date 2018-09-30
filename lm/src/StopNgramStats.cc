/*
 * StopNgramStats.cc --
 *	N-gram statistics with contexts excluding stop words
 *
 */

#ifndef lint
static char TaggedNgramStats_Copyright[] = "Copyright (c) 1996, SRI International.  All Rights Reserved.";
static char TaggedNgramStats_RcsId[] = "@(#)$Header: /home/srilm/devel/lm/src/RCS/StopNgramStats.cc,v 1.1 1996/12/10 09:28:59 stolcke Exp $";
#endif

#include <string.h>
#include <iostream.h>

#include "StopNgramStats.h"

StopNgramStats::StopNgramStats(Vocab &vocab, SubVocab &stopWords,
							unsigned maxOrder)
    : NgramStats(vocab, maxOrder), stopWords(stopWords)
{
}

void
StopNgramStats::incrementCounts(const VocabIndex *words)
{
    while (*words != Vocab_None) {
	counts.insertTrie(words ++)->value() ++;
    }
}

unsigned int
StopNgramStats::countSentence(const VocabIndex *words)
{
    unsigned sentLength = Vocab::length(words);

    VocabIndex *countWords = new VocabIndex[sentLength + 1];
    assert(countWords != 0);

    unsigned countPos = 0;
    for (unsigned nextPos = 0; nextPos < sentLength; nextPos++) {
	/*
	 * Count an ngram that has the current word as the last item,
	 * and is preceded the non-stop words found so far.
	 */
	countWords[countPos] = words[nextPos];
	countWords[countPos + 1] = Vocab_None;
	if (countPos + 1 >= order) {
	    incrementCounts(&countWords[countPos + 1 - order]);
	} else {
	    incrementCounts(countWords);
	}

	/*
	 * Check if the next word is a non-stop one, and if so
	 * include it in the context for the following ngrams
	 */
	if (!stopWords.getWord(words[nextPos])) {
	    countWords[countPos ++] = words[nextPos];
	}
    }

    /*
     * keep track of word and sentence counts
     */
    stats.numWords += sentLength;
    if (words[0] == vocab.ssIndex) {
	stats.numWords --;
    }
    if (sentLength > 0 && words[sentLength-1] == vocab.seIndex) {
	stats.numWords --;
    }

    stats.numSentences ++;

    delete [] countWords;
    return sentLength;
}

