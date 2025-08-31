#include <memory>
#include <iostream>
#include <fstream>
#include <vector>

class Generator {
public:
    typedef std::shared_ptr<Generator> ptr;
    Generator(std::string name, std::string np)
        :m_filename(name)
        ,m_namespace(np) {
    }

    void gen(const std::string& path);
    void gen_inc(const std::string& path);
    void gen_src(const std::string& path);
private:
    std::string GenMarco(const std::string& str);
    std::string GetClassName(const std::string& str);
private:
    std::string m_filename;
    std::string m_namespace;
};

void Generator::gen(const std::string& path) {
    std::cout << "generate begin" << std::endl;
    gen_inc(path);
    gen_src(path);
    std::cout << "generate end" << std::endl;
}

std::string Generator::GenMarco(const std::string& str) {
    std::string pjname = "blog/";
    std::string ret = "__";
    std::string s = pjname + (str.at(0) == '.' ? str.substr(2) : str);
    for (size_t i = 0; i < s.size(); ++i) {
        size_t j = i;
        std::string t;
        while (j < s.size() && s[j] != '/') {
            if (std::isalpha(s[j])) {
                s[j] = std::toupper(s[j]);
            } else if (s[j] == '.') {
                s[j] = '_';
            }
            t += s[j];
            ++j;
        }
        if (t == "SRC") {
            i = j;
            continue;
        } else {
            ret += t;
        }
        if (j >= s.size()) break;
        if (s[j] == '/') {
            ret += '_';
            i = j;
        }
    }
    ret += "__";
    return ret;
}

std::string Generator::GetClassName(const std::string& str) {
    std::string ret;
    std::string s = str;
    for (size_t i = 0; i < s.size(); ++i) {
        size_t j = i;
        std::string t;
        while (j < s.size() && s[j] != '_') {
            t += s[j];
            ++j;
        }
        t[0] = std::toupper(t[0]);
        ret += t;
        i = j;
    }
    return ret;
}

void Generator::gen_inc(const std::string& path) {
    std::string name = path + "/" + m_filename + ".h";
    std::ofstream ofs(name);
    
    std::string marco = GenMarco(name);
    ofs << "#ifndef " << marco << std::endl;
    ofs << "#define " << marco << std::endl;
    ofs << std::endl;

    std::vector<std::string> incs{"../struct.h"};
    for (size_t i = 0; i < incs.size(); ++i) {
        if (incs[i][0] != '<') {
            ofs << "#include " << "\"" << incs[i] << "\"" << std::endl;
        } else {
            ofs << "#include " << incs[i] << std::endl;
        }
    }
    ofs << std::endl;

    std::vector<std::string> ns;
    for (size_t i = 0; i < m_namespace.size(); ++i) {
        size_t j = i;
        std::string str;
        while (j < m_namespace.size() && m_namespace[j] != '.') {
            str += m_namespace[j];
            ++j;
        }
        ns.push_back(str);
        i = j;
    }

    for (size_t i = 0; i < ns.size(); ++i) {
        ofs << "namespace " << ns[i] << " {" << std::endl;
    }
    ofs << std::endl;
    
    std::string class_name = GetClassName(m_filename);
    ofs << "class " << class_name << " : public BlogServlet {" << std::endl;
    ofs << "public:" << std::endl;
    ofs << "    typedef std::shared_ptr<" << class_name << "> ptr;" << std::endl;
    ofs << "    " << class_name << "();" << std::endl;
    ofs << "    virtual int32_t handle(sylar::http::HttpRequest::ptr request" << std::endl;
    ofs << "                    ,sylar::http::HttpResponse::ptr response" << std::endl;
    ofs << "                    ,sylar::http::HttpSession::ptr session" << std::endl;
    ofs << "                    ,Result::ptr result) override;" << std::endl;
    ofs << "};"  << std::endl;
    ofs << std::endl;

    for (size_t i = 0; i < ns.size(); ++i) {
        ofs << "}" << std::endl;
    }
    ofs << std::endl;
    ofs << "#endif // " << marco;
}

void Generator::gen_src(const std::string& path) {
    std::string name = path + "/" + m_filename + ".cc";
    std::ofstream ofs(name);
    
    std::vector<std::string> incs{m_filename + ".h", "<chen/log/log.h>"};
    for (size_t i = 0; i < incs.size(); ++i) {
        ofs << "#include " << "\"" << incs[i] << "\"" << std::endl;
    }
    ofs << std::endl;

    std::vector<std::string> ns;
    for (size_t i = 0; i < m_namespace.size(); ++i) {
        size_t j = i;
        std::string str;
        while (j < m_namespace.size() && m_namespace[j] != '.') {
            str += m_namespace[j];
            ++j;
        }
        ns.push_back(str);
        i = j;
    }

    for (size_t i = 0; i < ns.size(); ++i) {
        ofs << "namespace " << ns[i] << " {" << std::endl;
    }
    ofs << std::endl;
    ofs << "static sylar::Logger::ptr logger = LOG_ROOT();" << std::endl;
    ofs << std::endl;

    std::string class_name = GetClassName(m_filename);
    ofs << class_name << "::" << class_name << "()" << std::endl;
    ofs << "    :BlogServlet(\"" << class_name << "\") {" << std::endl << "}" << std::endl;
    ofs << std::endl;

    ofs << "int32_t " << class_name << "::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response" << std::endl;
    ofs << "        ,sylar::http::HttpSession::ptr session, Result::ptr result) {" << std::endl;
    ofs << "    return 0;" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;

    for (size_t i = 0; i < ns.size(); ++i) {
        ofs << "}" << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "use as[" << argv[0] << " filename namespace]" << std::endl;
        return 0;
    }
    std::string out_path = "./src/servlets";
    std::string fln = argv[1];
    std::string nsp = argv[2];
    Generator::ptr G(std::make_shared<Generator>(fln, nsp));
    G->gen(out_path);

    return 0;
}