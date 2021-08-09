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

NgapAmfContext *NgapTask::selectAmf(int ueId)
{
    NgapAmfContext *retVal = NULL;
    // todo:
    for (auto &amf : m_amfCtx){
        m_logger->debug("AMF %s supports:", amf.second->amfName);
        for (auto &plmn : amf.second->plmnSupportList){
            m_logger->debug("%s", plmn->plmn);
            for (auto &snssai : plmn->sliceSupportList.slices){
                m_logger->debug("%d - %d", (uint8_t) snssai.sst, *(snssai.sd));
            }
        }
        if (!retVal)
            retVal = amf.second; // return the first one
    }
    return retVal;
}

NgapAmfContext *NgapTask::selectNewAmfForReAllocation(int ueId, int initiatedAmfId, int amfSetId)
{
    // TODO an arbitrary AMF is selected for now
    return findAmfContext(initiatedAmfId);
}

} // namespace nr::gnb
