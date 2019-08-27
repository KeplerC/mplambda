#pragma once
#ifndef MPL_DEMO_APP_OPTIONS_HPP
#define MPL_DEMO_APP_OPTIONS_HPP

#include <string>
#include <optional>
#include <Eigen/Dense>

namespace mpl::demo {
    template <class T>
    struct OptionParser;

    template <>
    struct OptionParser<float> {
        static float parse(const std::string& name, const char *arg, char **endp);
    };
    
    template <>
    struct OptionParser<double> {
        static double parse(const std::string& name, const char *arg, char **endp);
    };

    template <class S, int rows>
    struct OptionParser<Eigen::Matrix<S, rows, 1>> {
        static Eigen::Matrix<S, rows, 1> parse(const std::string& name, const char *arg, char **endp) {
            if (!*arg)
                throw std::invalid_argument(name + " is required");
            
            Eigen::Matrix<S, rows, 1> q;
            q[0] = OptionParser<S>::parse(name, arg, endp);
            for (int i=1 ; i<rows ; ++i) {
                if (**endp != ',')
                    throw std::invalid_argument("expected comma");
                q[i] = OptionParser<S>::parse(name, *endp + 1, endp);
            }
            return q;
        }
    };

    template <class S>
    struct OptionParser<Eigen::Quaternion<S>> {
        static Eigen::Quaternion<S> parse(const std::string& name, const char *arg, char **endp) {
            auto v = OptionParser<Eigen::Matrix<S, 4, 1>>::parse(name, arg, endp);
            // Eigen::Quaternion<S> q;
            // q = Eigen::AngleAxis<S>{v[0], v.template tail<3>().normalized()};
            // return q;
            return Eigen::Quaternion<S>{v};
        }
    };

    template <class A, class B>
    struct OptionParser<std::tuple<A, B>> {
        static std::tuple<A, B> parse(const std::string& name, const char *arg, char **endp) {
            A a = OptionParser<A>::parse(name, arg, endp);
            if (**endp != ',')
                throw std::invalid_argument("expected comma");
            return { a, OptionParser<B>::parse(name, *endp + 1, endp) };
        }
    };

    template <class T>
    struct OptionParser<std::optional<T>> {
        static std::optional<T> parse(const std::string& name, const char *arg, char **endp) {
            if (*arg)
                return OptionParser<T>::parse(name, arg, endp);
            else
                return {};
        }
    };
    
    class AppOptions {
        std::string algorithm_;
        std::string coordinator_;
        std::uint64_t problemId_;

        std::string env_;
        std::string robot_;

        std::string start_;
        std::string goal_;

        std::string min_;
        std::string max_;

        double timeLimit_{std::numeric_limits<double>::infinity()};
        double discretization_{0};

        static void usage(const char *argv0);

        template <class T>
        static T parse(const std::string& name, const std::string& value) {
            char *endp;
            T r = OptionParser<T>::parse(name, value.c_str(), &endp);
            if (*endp)
                throw std::invalid_argument("extra characters in --" + name);
            return r;
        }
        
    public:
        AppOptions(int argc, char* argv[]);

        const std::string& algorithm(bool required = true) const {
            if (required && algorithm_.empty())
                throw std::invalid_argument("--algorithm is required");
            return algorithm_;
        }
        
        const std::string& coordinator(bool required = true) const {
            if (required && coordinator_.empty())
                throw std::invalid_argument("--coordinator is required");
            return coordinator_;
        }
        
        const std::uint64_t problemId() const {
            return problemId_;
        }
        
        const std::string& env(bool required = true) const {
            if (required && env_.empty())
                throw std::invalid_argument("--env is required");
            return env_;
        }
        
        const std::string& robot(bool required = true) const {
            if (required && robot_.empty())
                throw std::invalid_argument("--robot is required");
            return robot_;
        }

        template <class T>
        T start() const {
            return parse<T>("start", start_);
        }

        template <class T>
        T goal() const {
            return parse<T>("goal", goal_);
        }
        
        template <class T>
        T min() const {
            return parse<T>("min", min_);
        }

        template <class T>
        T max() const {
            return parse<T>("max", max_);
        }

        double timeLimit() const {
            return timeLimit_;
        }

        double discretization() const {
            return discretization_;
        }
    };
}

#endif