// Copyright (c) 2014-2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "checkpoints.h"

#include "common/dns_utils.h"
#include "string_tools.h"
#include "storages/portable_storage_template_helper.h" // epee json include
#include "serialization/keyvalue_serialization.h"
#include <functional>
#include <vector>

using namespace epee;

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "checkpoints"

namespace cryptonote
{
  /**
   * @brief struct for loading a checkpoint from json
   */
  struct t_hashline
  {
    uint64_t height; //!< the height of the checkpoint
    std::string hash; //!< the hash for the checkpoint
        BEGIN_KV_SERIALIZE_MAP()
          KV_SERIALIZE(height)
          KV_SERIALIZE(hash)
        END_KV_SERIALIZE_MAP()
  };

  /**
   * @brief struct for loading many checkpoints from json
   */
  struct t_hash_json {
    std::vector<t_hashline> hashlines; //!< the checkpoint lines from the file
        BEGIN_KV_SERIALIZE_MAP()
          KV_SERIALIZE(hashlines)
        END_KV_SERIALIZE_MAP()
  };

  //---------------------------------------------------------------------------
  checkpoints::checkpoints()
  {
  }
  //---------------------------------------------------------------------------
  bool checkpoints::add_checkpoint(uint64_t height, const std::string& hash_str, const std::string& difficulty_str)
  {
    crypto::hash h = crypto::null_hash;
    bool r = epee::string_tools::hex_to_pod(hash_str, h);
    CHECK_AND_ASSERT_MES(r, false, "Failed to parse checkpoint hash string into binary representation!");

    // return false if adding at a height we already have AND the hash is different
    if (m_points.count(height))
    {
      CHECK_AND_ASSERT_MES(h == m_points[height], false, "Checkpoint at given height already exists, and hash for new checkpoint was different!");
    }
    m_points[height] = h;
    if (!difficulty_str.empty())
    {
      try
      {
        difficulty_type difficulty(difficulty_str);
        if (m_difficulty_points.count(height))
        {
          CHECK_AND_ASSERT_MES(difficulty == m_difficulty_points[height], false, "Difficulty checkpoint at given height already exists, and difficulty for new checkpoint was different!");
        }
        m_difficulty_points[height] = difficulty;
      }
      catch (...)
      {
        LOG_ERROR("Failed to parse difficulty checkpoint: " << difficulty_str);
        return false;
      }
    }
    return true;
  }
  //---------------------------------------------------------------------------
  bool checkpoints::is_in_checkpoint_zone(uint64_t height) const
  {
    return !m_points.empty() && (height <= (--m_points.end())->first);
  }
  //---------------------------------------------------------------------------
  bool checkpoints::check_block(uint64_t height, const crypto::hash& h, bool& is_a_checkpoint) const
  {
    auto it = m_points.find(height);
    is_a_checkpoint = it != m_points.end();
    if(!is_a_checkpoint)
      return true;

    if(it->second == h)
    {
      MINFO("CHECKPOINT PASSED FOR HEIGHT " << height << " " << h);
      return true;
    }else
    {
      MWARNING("CHECKPOINT FAILED FOR HEIGHT " << height << ". EXPECTED HASH: " << it->second << ", FETCHED HASH: " << h);
      return false;
    }
  }
  //---------------------------------------------------------------------------
  bool checkpoints::check_block(uint64_t height, const crypto::hash& h) const
  {
    bool ignored;
    return check_block(height, h, ignored);
  }
  //---------------------------------------------------------------------------
  //FIXME: is this the desired behavior?
  bool checkpoints::is_alternative_block_allowed(uint64_t blockchain_height, uint64_t block_height) const
  {
    if (0 == block_height)
      return false;

    auto it = m_points.upper_bound(blockchain_height);
    // Is blockchain_height before the first checkpoint?
    if (it == m_points.begin())
      return true;

    --it;
    uint64_t checkpoint_height = it->first;
    return checkpoint_height < block_height;
  }
  //---------------------------------------------------------------------------
  uint64_t checkpoints::get_max_height() const
  {
    if (m_points.empty())
      return 0;
    return m_points.rbegin()->first;
  }
  //---------------------------------------------------------------------------
  const std::map<uint64_t, crypto::hash>& checkpoints::get_points() const
  {
    return m_points;
  }
  //---------------------------------------------------------------------------
  const std::map<uint64_t, difficulty_type>& checkpoints::get_difficulty_points() const
  {
    return m_difficulty_points;
  }

  bool checkpoints::check_for_conflicts(const checkpoints& other) const
  {
    for (auto& pt : other.get_points())
    {
      if (m_points.count(pt.first))
      {
        CHECK_AND_ASSERT_MES(pt.second == m_points.at(pt.first), false, "Checkpoint at given height already exists, and hash for new checkpoint was different!");
      }
    }
    return true;
  }

  bool checkpoints::init_default_checkpoints(network_type nettype)
  {
    if (nettype == TESTNET)
    {
      return true;
    }
    if (nettype == STAGENET)
    {
      return true;
    }
    ADD_CHECKPOINT2(1,       "f48e334a588b89260bd75a1925df0f145751cda3025b713c73aa78232da3e4d6","0x2");
    ADD_CHECKPOINT2(100,     "3ecd27aded3269d142128aab5af78fc91c7fa2c7adf63e58844208b46fa83b58","0x3504B");
    ADD_CHECKPOINT2(10000,   "ee54ea5924ffa8677290620a6ddc865224ea8a297e7c08a10dbb62d156e2a1ec","0x2712D3E7B");
    ADD_CHECKPOINT2(50000,   "cc3e8c228df95fe3abedeb0339ef8ae425b592b6ce61ee853a87b8aad20b69b7","0x1F2F615B3EA");
    ADD_CHECKPOINT2(75000,   "d9a01e54ebc9fb720e326fcc10c9a7a83e64c339a038fb33e420e4d913aa0dce","0x43BD58C6FEC");
    ADD_CHECKPOINT2(100000,  "1d69279e55a88120db679e7ecceb5415424623422609d86f6a2fbdb892a55b93","0x6ACDF20304C");
    ADD_CHECKPOINT2(110000,  "019d822a299f2c5c7acfc19b32649c3add8e0697ea6e1592d3c46cd2036ca2f3","0x77748FED217");
    ADD_CHECKPOINT2(150000,  "b9cbaa0bccd30478f26fe4f6177b66c010cbf2b589c9c44c82a9794bce5ed418","0xA5AB15EEE01");
    ADD_CHECKPOINT2(300000,  "85fce3b66fd8113aace405957b3459c54dd39d9b5eb6976ecdfec9e750399562","0x1529FF8C8905");
    ADD_CHECKPOINT2(400000,  "1ed1e3fd9174be998ac4f446dcfc91080477a4a790acf5d199fc633b9d76af01","0x1AC1A7D1CCEB");
    ADD_CHECKPOINT2(500000,  "4a5e0b52efa1b922e793d1d5b5f77930c55c19770b5f5e680ee816f5bc2a7c97","0x2034D7D86291");
    ADD_CHECKPOINT2(600000,  "0a42fe6a40f5b084fb63c77c0d76e9e274a78687397af5cc6bbb27a9458d9afb","0x24F52E832797");
    ADD_CHECKPOINT2(700000,  "a0a62377dc0ea9d869b0c381ffe19b18ea0434bb720390f4bd57b509f252289d","0x24F551CF20A0");
    ADD_CHECKPOINT2(800000,  "62207aad524bdcc3572f1e1d778a64bd571c90cc4f0c20fa48a59b30eac42ed8","0x24F591211BE7");
    ADD_CHECKPOINT2(1000000, "bd44362fdc5a0e3f83765fa26a85f0b395bb790e21ff25a9ed802cd8a94e91b4","0x24F601E4388B");
    ADD_CHECKPOINT2(1250000, "7a53910047eb12b347375b75213ed7b28656fccc76281002f759e676a7931396","0x24F63CC490DE");
    ADD_CHECKPOINT2(1500000, "4c0a13efaa9f39e5e870621d39aaa7baf33c9c27a137a9a20d7ba57332a9f588","0x24F6D4997FF4");
    ADD_CHECKPOINT2(1750000, "a06ad4b21cd3b9d5544f62f77c3dd163dc240913252582dfc75fab829ab0d149","0x24F78B4023F1");
    ADD_CHECKPOINT2(2000000, "2a5920d297d8e1e88bc0845d6b964c46009d78266258596c84dff446c9a1042c","0x24F7E904CEEC");
    ADD_CHECKPOINT2(2250000, "e58da5b896cb9e923a8daf7d7f7d670636d2fb8e9b8b7ae723b1ddfd09068e93","0x24F837A95DA3");
    ADD_CHECKPOINT2(2500000, "70791ca10ff16168dca78076efa67f2153c4ff68fe5b92d100d16fca7966bfb3","0x24F87E038F7C");
    ADD_CHECKPOINT2(2750000, "7b7106b23809716b4a9e0e6ee881fa6272ac15f2f42522d0666d4a6aa31aad78","0x24F8AF51F7A2");
    ADD_CHECKPOINT2(3000000, "04f18897bb02bc6275069b6cb460d34ca7df011e6a913b5efec0557f8639086f","0x24F8D34B21D0");
    ADD_CHECKPOINT2(3250000, "5535b32b30fe6896de4c966962006bdc4a9fc706f3f4d31ec35cfa9edacbc41e","0x24F8EEC16082");
    ADD_CHECKPOINT2(3500000, "551e50f2088bb66715a0ba3e8cc80af6d159d4f671426064382a74503eb0f4b3","0x24F909221282");
    ADD_CHECKPOINT2(3616348, "e833befdc5ccb515d2fc4337c314bfcece32887cbc6ec619770599ae02c13122","0x24F911C83D1D");
    ADD_CHECKPOINT2(3619107, "b42aa409b05d7c0d399e67391b3f332f590dda2c92611bf82ea1850ccaabff2a","0x24F911F345ED");
    ADD_CHECKPOINT2(8618276, "bc3e927cb1b24893264554a655671b31d5e938204c3c25b13eb0c18594d07e80","0x24F97598B380");
    ADD_CHECKPOINT2(13305079, "f8aa74925f361d4747d8d25a0cfd02960d15dc401bd7f4be621258b3119b6b9e","0x24F992DBF446");
    ADD_CHECKPOINT2(15300237, "c7e6d4ba695ed1f495e5ab7a8d883003e1fb2c9ada8afc5211e13c01631f4b16","0X24F993DD2AD0")
    return true;
  }

  bool checkpoints::load_checkpoints_from_json(const std::string &json_hashfile_fullpath)
  {
    boost::system::error_code errcode;
    if (! (boost::filesystem::exists(json_hashfile_fullpath, errcode)))
    {
      LOG_PRINT_L1("Blockchain checkpoints file not found");
      return true;
    }

    LOG_PRINT_L1("Adding checkpoints from blockchain hashfile");

    uint64_t prev_max_height = get_max_height();
    LOG_PRINT_L1("Hard-coded max checkpoint height is " << prev_max_height);
    t_hash_json hashes;
    if (!epee::serialization::load_t_from_json_file(hashes, json_hashfile_fullpath))
    {
      MERROR("Error loading checkpoints from " << json_hashfile_fullpath);
      return false;
    }
    for (std::vector<t_hashline>::const_iterator it = hashes.hashlines.begin(); it != hashes.hashlines.end(); )
    {
      uint64_t height;
      height = it->height;
      if (height <= prev_max_height) {
	LOG_PRINT_L1("ignoring checkpoint height " << height);
      } else {
	std::string blockhash = it->hash;
	LOG_PRINT_L1("Adding checkpoint height " << height << ", hash=" << blockhash);
	ADD_CHECKPOINT(height, blockhash);
      }
      ++it;
    }

    return true;
  }

  bool checkpoints::load_checkpoints_from_dns(network_type nettype)
  {
    std::vector<std::string> records;

    // All four MoneroPulse domains have DNSSEC on and valid
    static const std::vector<std::string> dns_urls = {
    };

    static const std::vector<std::string> testnet_dns_urls = {
    };

    static const std::vector<std::string> stagenet_dns_urls = {
    };

    if (!tools::dns_utils::load_txt_records_from_dns(records, nettype == TESTNET ? testnet_dns_urls : nettype == STAGENET ? stagenet_dns_urls : dns_urls))
      return true; // why true ?

    for (const auto& record : records)
    {
      auto pos = record.find(":");
      if (pos != std::string::npos)
      {
        uint64_t height;
        crypto::hash hash;

        // parse the first part as uint64_t,
        // if this fails move on to the next record
        std::stringstream ss(record.substr(0, pos));
        if (!(ss >> height))
        {
    continue;
        }

        // parse the second part as crypto::hash,
        // if this fails move on to the next record
        std::string hashStr = record.substr(pos + 1);
        if (!epee::string_tools::hex_to_pod(hashStr, hash))
        {
    continue;
        }

        ADD_CHECKPOINT(height, hashStr);
      }
    }
    return true;
  }

  bool checkpoints::load_new_checkpoints(const std::string &json_hashfile_fullpath, network_type nettype, bool dns)
  {
    bool result;

    result = load_checkpoints_from_json(json_hashfile_fullpath);
    if (dns)
    {
      result &= load_checkpoints_from_dns(nettype);
    }

    return result;
  }
}
