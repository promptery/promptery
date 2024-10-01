#pragma once

#include <functional>

class finally
{
public:
    explicit finally(std::function<void()> f)
        : m_finallyClause(std::move(f))
    {
    }

    void reset(std::function<void()> f = nullptr) { m_finallyClause = std::move(f); }

    ~finally()
    {
        if (m_finallyClause) {
            m_finallyClause();
        }
    }

private:
    std::function<void()> m_finallyClause;

    finally(const finally &);
    finally &operator=(const finally &);
};
