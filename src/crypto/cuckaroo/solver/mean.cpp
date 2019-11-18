// Cuckaroo Cycle, a memory-hard proof-of-work
// Copyright (c) 2013-2019 John Tromp

#include "mean.hpp"
#include <unistd.h>
#include <chrono>

typedef solver_ctx SolverCtx;

CALL_CONVENTION SolverCtx* create_solver_ctx(SolverParams* params) {
  if (params->nthreads == 0) params->nthreads = 1;
  if (params->ntrims == 0) params->ntrims = EDGEBITS >= 30 ? 96 : 68;

  SolverCtx* ctx = new SolverCtx(params->nthreads,
                                 params->ntrims,
                                 params->allrounds,
                                 params->showcycle,
                                 params->mutate_nonce);
  return ctx;
}

CALL_CONVENTION void destroy_solver_ctx(SolverCtx* ctx) {
  delete ctx;
}

CALL_CONVENTION void stop_solver(SolverCtx* ctx) {
  ctx->abort();
}

CALL_CONVENTION void fill_default_params(SolverParams* params) {
	// not required in this solver
}

void c29_find_edges(const void* in, size_t len, uint32_t nonce, uint32_t* edges) {

  int c;
  //u64 time0, time1;
  //u32 timems;

  char header[255];

  for(uint32_t i = 0;i < len;i++)
    header[i]=((unsigned char*)in)[i];

  header[len+3] = nonce & 0xff;
  header[len+2] = (nonce >> 8 ) & 0xff ;
  header[len+1] = (nonce >> 16 ) & 0xff ;
  header[len]   = (nonce >> 24 ) & 0xff ;

  SolverParams params;
  params.nthreads = 0;
  params.ntrims = 0;
  params.showcycle = 1;
  params.allrounds = false;

  SolverCtx* ctx = create_solver_ctx(&params);

//  print_log("Looking for %d-cycle on cuckaroo%d(%ul) with 50%% edges\n", PROOFSIZE, EDGEBITS, nonce);

  u64 sbytes = ctx->sharedbytes();
  u32 tbytes = ctx->threadbytes();
  int sunit,tunit;
  for (sunit=0; sbytes >= 10240; sbytes>>=10,sunit++) ;
  for (tunit=0; tbytes >= 10240; tbytes>>=10,tunit++) ;
//  print_log("Using %d%cB bucket memory at %lx,\n", sbytes, " KMGT"[sunit], (u64)ctx->trimmer.buckets);
//  print_log("%dx%d%cB thread memory at %lx,\n", params.nthreads, tbytes, " KMGT"[tunit], (u64)ctx->trimmer.tbuckets);
//  print_log("%d buckets.\n", NX);


  //time0 = timestamp();
  ctx->setheadernonce(header, len+4);
//  print_log("nonce k0 k1 k2 k3 %llx %llx %llx %llx\n", ctx->trimmer.sip_keys.k0, ctx->trimmer.sip_keys.k1, ctx->trimmer.sip_keys.k2, ctx->trimmer.sip_keys.k3);
  u32 nsols = ctx->solve();
  //time1 = timestamp();
  //timems = (time1 - time0) / 1000000;
  //print_log("Time: %d ms\n", timems);

  for (unsigned s = 0; s < nsols; s++) {
    //print_log("Solution");
    word_t *prf = &ctx->sols[s * PROOFSIZE];
    //for (u32 i = 0; i < PROOFSIZE; i++)
    //  print_log(" %jx", (uintmax_t)prf[i]);
    //print_log("\n");
    int pow_rc = verify(prf, ctx->trimmer.sip_keys);
    if (pow_rc == POW_OK) {
      //print_log("Verified with cyclehash ");
      //unsigned char cyclehash[32];
      //blake2b((void *)cyclehash, sizeof(cyclehash), (const void *)prf, sizeof(proof), 0, 0);
      //for (int i=0; i<32; i++)
      //  print_log("%02x", cyclehash[i]);
      for(int i = 0; i < 32; i++) edges[i] = prf[i];
      //print_log("\n");
    } else {
      //print_log("FAILED due to %s\n", errstr[pow_rc]);
    }
  }

  destroy_solver_ctx(ctx);
}
