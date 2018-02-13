#include "mcrclient.h"
#include "mcrclientimpl.h"

MCRClient* MCRClientFactory::CreateInstance()
{
    return (new MCRClientImpl);
}
