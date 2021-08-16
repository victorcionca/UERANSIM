//
// This file is a part of UERANSIM open source project.
// Copyright (c) 2021 ALİ GÜNGÖR.
//
// The software and all associated files are licensed under GPL-3.0
// and subject to the terms and conditions defined in LICENSE file.
//

#include "task.hpp"

namespace nr::gnb
{

NgapAmfContext *NgapTask::selectAmf(int ueId, NetworkSlice &ueNssai)
{
    // todo:
    NgapAmfContext *retVal = NULL;
    for (auto &amf : m_amfCtx){
        for (auto &plmn : amf.second->plmnSupportList){
            for (auto &snssai : plmn->sliceSupportList.slices){
                for (auto &ueSNssai : ueNssai.slices){
                    if (snssai == ueSNssai){
                        m_logger->debug("Found match on [%d %x] -- [%d %x]",
                                ueSNssai.sst, ueSNssai.sd.value(),
                                snssai.sst, snssai.sd.value());
                        m_logger->debug("AMF address: %s.%d",
                                amf.second->address.c_str(),
                                amf.second->port);
                        retVal = amf.second;
                    }
                }
            }
        }
    }
    return retVal;
}

NgapAmfContext *NgapTask::selectNewAmfForReAllocation(int ueId, int initiatedAmfId, int amfSetId)
{
    // TODO an arbitrary AMF is selected for now
    return findAmfContext(initiatedAmfId);
}

} // namespace nr::gnb
