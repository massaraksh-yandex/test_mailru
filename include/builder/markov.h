#pragma once

#include <common/markov.h>
#include <common/log.h>

#include <builder/read_chunk.h>


namespace builder {

ReadChunkResult::Words insertToMarkov(common::Markov& net, ReadChunkResult::Words words);
void lastInsertToMarkov(common::Markov& net, ReadChunkResult::Words words);

}
