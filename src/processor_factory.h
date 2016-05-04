/**
 * \file processor_factory.h
 *
 *  Created on: May 4, 2016
 *      \author: jsola
 */

#ifndef PROCESSOR_FACTORY_H_
#define PROCESSOR_FACTORY_H_

#include "processor_base.h"

namespace wolf
{

class ProcessorFactory
{
    public:
        typedef ProcessorBase* (*CreateProcessorCallback)(const std::string & _unique_name, const ProcessorParamsBase* _params);
    private:
        typedef std::map<std::string, CreateProcessorCallback> CallbackMap;
    public:
        bool registerCreator(const std::string& _processor_type, CreateProcessorCallback createFn);
        bool unregisterCreator(const std::string& _processor_type);
        ProcessorBase* create(const std::string& _processor_type, const std::string& _unique_name, const ProcessorParamsBase* _params);
    private:
        CallbackMap callbacks_;

        // Singleton ---------------------------------------------------
        // This class is a singleton. The code below guarantees this.
    public:
        static ProcessorFactory* get(); // Unique point of access
    private:
        ProcessorFactory(); // Prevent clients from creating a new Singleton
        ProcessorFactory(const ProcessorFactory&); // Prevent clients from creating a copy of the Singleton
        static ProcessorFactory* pInstance_;
};

} /* namespace wolf */

#endif /* PROCESSOR_FACTORY_H_ */
