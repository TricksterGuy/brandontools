#ifndef FORTUNE_GEN_HPP
#define FORTUNE_GEN_HPP

#include <string>

class FortuneGenerator
{
	public:
		~FortuneGenerator();
		static FortuneGenerator& Instance()
		{
		    static FortuneGenerator gen;
		    return gen;
		}
		const std::string GetFortune() const;
		const std::string GetQuote() const;
	private:
        int quote;
        int fortune;
        FortuneGenerator();
        FortuneGenerator(const FortuneGenerator&);
        const FortuneGenerator& operator=(const FortuneGenerator& o);

};


#endif
