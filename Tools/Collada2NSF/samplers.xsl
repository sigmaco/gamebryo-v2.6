<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">

  <!-- Process Samplers -->
  <xsl:template name="Sampler">
    <xsl:for-each select="./col:bind/col:param">

      <xsl:variable name="Ref" select="@ref"/>
      <xsl:variable name="Profile" select="ancestor::col:profile_CG"/>
      <xsl:variable name="Sampler" select="$Profile/col:newparam[@sid=$Ref]/col:sampler2D"/>
      <xsl:variable name="Semantic" select="$Profile/col:newparam[@sid=$Ref]/col:semantic"/>
      <xsl:variable name="NTMFlag" select="normalize-space($Profile/col:newparam[@sid=$Sampler/col:source]/col:annotate[@name='NTM']/col:string)"/>

      <!-- Convert Min filter to Min and Map filters (which is expected internally) -->
      <xsl:variable name="MinFilter" select="$Sampler/col:minfilter"/>
      <xsl:variable name="MinFilterValue">
        <xsl:choose>
          <xsl:when test="$MinFilter='LINEAR'">LINEAR</xsl:when>
          <xsl:when test="$MinFilter='NEAREST'">POINT</xsl:when>
          <xsl:when test="$MinFilter='LINEAR_MIPMAP_LINEAR'">LINEAR</xsl:when>
          <xsl:when test="$MinFilter='NEAREST_MIPMAP_LINEAR'">POINT</xsl:when>
          <xsl:when test="$MinFilter='NEAREST_MIPMAP_NEAREST'">POINT</xsl:when>
          <xsl:when test="$MinFilter='LINEAR_MIPMAP_NEAREST'">LINEAR</xsl:when>
          <xsl:otherwise>NONE</xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:variable name="MipFilterValue">
        <xsl:choose>
          <xsl:when test="$MinFilter='LINEAR_MIPMAP_LINEAR'">LINEAR</xsl:when>
          <xsl:when test="$MinFilter='NEAREST_MIPMAP_LINEAR'">LINEAR</xsl:when>
          <xsl:when test="$MinFilter='NEAREST_MIPMAP_NEAREST'">POINT</xsl:when>
          <xsl:when test="$MinFilter='LINEAR_MIPMAP_NEAREST'">POINT</xsl:when>
          <xsl:otherwise>NONE</xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:variable name="MagFilter" select="$Sampler/col:magfilter"/>
      <xsl:variable name="MagFilterValue">
        <xsl:choose>
          <xsl:when test="$MagFilter='NEAREST'">POINT</xsl:when>
          <xsl:when test="$MagFilter='LINEAR'">LINEAR</xsl:when>
          <xsl:otherwise>NONE</xsl:otherwise>
          </xsl:choose>
      </xsl:variable>

      <xsl:if test="count($Sampler)=1">

        <xsl:variable name="SamplerIndex" select="substring-after($Semantic, 'TEXUNIT')"/>

        <xsl:value-of select="concat($Tabs3, 'Sampler ', $SamplerIndex, ' ', $Ref, '&#xA;')"/>
        <xsl:value-of select="concat($Tabs3, '{&#xA;')"/>

        <xsl:choose>
          <xsl:when test="$NTMFlag!='' and $NTMFlag!='shader'">
            <xsl:value-of select="concat($Tabs4, 'TSS_Texture = NTM_', translate($NTMFlag, $l, $u), '&#xA;')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="concat($Tabs4, 'TSS_Texture = ', '&lt;', $Sampler/col:source, '&gt;&#xA;')"/>
          </xsl:otherwise>
        </xsl:choose>
        
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">TSAMP_AddressU</xsl:with-param>
          <xsl:with-param name="prefix">TADDR</xsl:with-param>
          <xsl:with-param name="value" select="$Sampler/col:wrap_s"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">TSAMP_AddressV</xsl:with-param>
          <xsl:with-param name="prefix">TADDR</xsl:with-param>
          <xsl:with-param name="value" select="$Sampler/col:wrap_t"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">TSAMP_AddressW</xsl:with-param>
          <xsl:with-param name="prefix">TADDR</xsl:with-param>
          <xsl:with-param name="value" select="$Sampler/col:wrap_p"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">TSAMP_MinFilter</xsl:with-param>
          <xsl:with-param name="prefix">TEXF</xsl:with-param>
          <xsl:with-param name="value" select="$MinFilterValue"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">TSAMP_MagFilter</xsl:with-param>
          <xsl:with-param name="prefix">TEXF</xsl:with-param>
          <xsl:with-param name="value" select="$MagFilterValue"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">TSAMP_MipFilter</xsl:with-param>
          <xsl:with-param name="prefix">TEXF</xsl:with-param>
          <xsl:with-param name="value" select="$MipFilterValue"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">//TSAMP_MaxLevel</xsl:with-param>
          <xsl:with-param name="prefix">TEXF</xsl:with-param>
          <xsl:with-param name="value" select="$Sampler/col:mipmap_maxlevel"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">//TSAMP_MipBias</xsl:with-param>
          <xsl:with-param name="prefix">TEXF</xsl:with-param>
          <xsl:with-param name="value" select="$Sampler/col:mipmap_bias"/>
        </xsl:call-template>
        <xsl:call-template name="ProcessSamplerState">
          <xsl:with-param name="label">//TSAMP_BorderColor</xsl:with-param>
          <xsl:with-param name="prefix">TEXF</xsl:with-param>
          <xsl:with-param name="value" select="$Sampler/col:border_color"/>
        </xsl:call-template>
        <xsl:value-of select="concat($Tabs3, '}&#xA;')"/>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <!-- Adds a single processor sample state if present -->
  <xsl:template name="ProcessSamplerState">
    <xsl:param name="label"/>
    <xsl:param name="prefix"/>
    <xsl:param name="value"/>
    <xsl:if test="$value!=''">
      <xsl:value-of select="concat($Tabs4, $label, ' = ', $prefix, '_', $value, '&#xA;')"/>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
