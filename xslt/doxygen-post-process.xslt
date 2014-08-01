<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
              xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
              version="1.0">
  <xsl:output method="xml" encoding="UTF-8"
              indent="no"
              media-type="application/xml" />

  <xsl:strip-space elements="*"/>

  <!-- Taken from: http://geekswithblogs.net/Erik/archive/2008/04/01/120915.aspx -->
  <xsl:template name="string-replace-all">
    <xsl:param name="text" />
    <xsl:param name="replace" />
    <xsl:param name="by" />
    <xsl:choose>
      <xsl:when test="contains($text, $replace)">
        <xsl:value-of select="substring-before($text,$replace)" />
        <xsl:value-of select="$by" />
        <xsl:call-template name="string-replace-all">
          <xsl:with-param name="text"
          select="substring-after($text,$replace)" />
          <xsl:with-param name="replace" select="$replace" />
          <xsl:with-param name="by" select="$by" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Doxygen does some strange and rather ugly post-processing on identifiers,
       so we recover the originals. --> 
  <xsl:template name="recover-id">
    <xsl:param name="original"/>
    <xsl:call-template name="string-replace-all">
      <xsl:with-param name="text"><xsl:call-template name="string-replace-all">
          <xsl:with-param name="text"><xsl:call-template name="string-replace-all">
              <xsl:with-param name="text" select="$original" />
              <xsl:with-param name="replace" select="'_8'" />
              <xsl:with-param name="by" select="'.'" />
            </xsl:call-template></xsl:with-param>
          <xsl:with-param name="replace" select="'_1'" />
          <xsl:with-param name="by" select="':'" />
        </xsl:call-template></xsl:with-param>
      <xsl:with-param name="replace" select="'_'" />
      <xsl:with-param name="by" select="'-'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="fix-path">
    <xsl:param name="original"/>
    <xsl:choose>
      <xsl:when test="substring-after($original,'include')">include<xsl:value-of select="substring-after($original,'include')"/></xsl:when>
      <xsl:when test="substring-after($original,'src')">src<xsl:value-of select="substring-after($original,'src')"/></xsl:when>
      <xsl:when test="substring-after($original,'documentation')">documentation<xsl:value-of select="substring-after($original,'documentation')"/></xsl:when>
      <xsl:when test="substring-after($original,'README')">README<xsl:value-of select="substring-after($original,'README')"/></xsl:when>
      <xsl:otherwise><xsl:value-of select="$original"/></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()" />
    </xsl:copy>
  </xsl:template>

  <xsl:template match="@id">
    <xsl:attribute name="id"><xsl:call-template name="recover-id">
        <xsl:with-param name="original" select="." />
      </xsl:call-template></xsl:attribute>
  </xsl:template>

  <xsl:template match="@refid">
    <xsl:attribute name="refid"><xsl:call-template name="recover-id">
        <xsl:with-param name="original" select="." />
      </xsl:call-template></xsl:attribute>
  </xsl:template>

  <xsl:template match="@file">
    <xsl:attribute name="file"><xsl:call-template name="fix-path">
        <xsl:with-param name="original" select="." />
      </xsl:call-template></xsl:attribute>
  </xsl:template>

  <xsl:template match="@bodyfile">
    <xsl:attribute name="bodyfile"><xsl:call-template name="fix-path">
        <xsl:with-param name="original" select="." />
      </xsl:call-template></xsl:attribute>
  </xsl:template>

  <xsl:template match="@external">
    <xsl:attribute name="external"><xsl:call-template name="fix-path">
        <xsl:with-param name="original" select="." />
      </xsl:call-template></xsl:attribute>
  </xsl:template>
</xsl:stylesheet>

